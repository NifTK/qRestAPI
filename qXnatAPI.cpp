/*==============================================================================

  Program: qRestAPI

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// qXnatAPI includes
#include "qXnatAPI.h"
#include "qXnatAPI_p.h"
#include "qRestResult.h"
#include <qNetworkReply>
#include <QRegExp>
#include <QUrl>
#include <qDebug>

// --------------------------------------------------------------------------
// qXnatAPIPrivate methods

// --------------------------------------------------------------------------
qXnatAPIPrivate::qXnatAPIPrivate(qXnatAPI* object)
  : Superclass(object)
  , q_ptr(object)
{
}

// --------------------------------------------------------------------------
QList<QVariantMap> qXnatAPIPrivate::parseXmlResponse(qRestResult* restResult, const QByteArray& response)
{
  QList<QVariantMap> result;
  return result;
}

// --------------------------------------------------------------------------
QList<QVariantMap> qXnatAPIPrivate::parseJsonResponse(qRestResult* restResult, const QByteArray& response)
{
  Q_Q(qXnatAPI);

  QScriptValue scriptValue = this->ScriptEngine.evaluate("(" + QString(response) + ")");

  QList<QVariantMap> result;

  // e.g. {"ResultSet":{"Result": [{"p1":"v1","p2":"v2",...}], "totalRecords":"13"}}
  QScriptValue resultSet = scriptValue.property("ResultSet");
  QScriptValue dataLength = resultSet.property("totalRecords");
  QScriptValue data = resultSet.property("Result");
  if (!data.isObject())
    {
    if (data.toString().isEmpty())
      {
      restResult->setError("No data");
      }
    else
      {
      restResult->setError(QString("Bad data: ") + data.toString());
      }
    }
  if (data.isArray())
    {
    quint32 length = data.property("length").toUInt32();
    for(quint32 i = 0; i < length; ++i)
      {
      qRestAPI::appendScriptValueToVariantMapList(result, data.property(i));
      }
    }
  else
    {
    qRestAPI::appendScriptValueToVariantMapList(result, data);
    }

  return result;
}

// --------------------------------------------------------------------------
// qXnatAPI methods

// --------------------------------------------------------------------------
qXnatAPI::qXnatAPI(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qXnatAPIPrivate(this))
{
}

// --------------------------------------------------------------------------
qXnatAPI::~qXnatAPI()
{
}

// --------------------------------------------------------------------------
QUuid qXnatAPI::get(const QString& resource, const Parameters& parameters, const qRestAPI::RawHeaders& rawHeaders)
{
  Q_D(qXnatAPI);
  QUrl url = this->createUrl(resource, parameters);
  url.addQueryItem("format", "json");
  QNetworkReply* queryReply = this->sendRequest(QNetworkAccessManager::GetOperation, url, rawHeaders);
  QUuid queryId = queryReply->property("uuid").toString();
  return queryId;
}

// --------------------------------------------------------------------------
void qXnatAPI::parseResponse(qRestResult* restResult, const QByteArray& response)
{
  Q_D(qXnatAPI);

  static QRegExp identifierPattern("[a-zA-Z][a-zA-Z0-9_]*");

  QList<QVariantMap> result;

  if (response.isEmpty())
    {
    // Some operations do not return result. E.g. creating a project.
    }
  else if (response.startsWith("<html>"))
    {
    // Some operations return an XML description of an object.
    // E.g. GET query for a specific subject.
    qDebug() << "HELLO 1";
    restResult->setError(QString("Bad data: ") + response);
    }
  else if (response.startsWith("<?xml "))
    {
    // Some operations return an XML description of an object.
    // E.g. GET query for a specific subject.
    qDebug() << "HELLO 2";
    result = d->parseXmlResponse(restResult, response);
    }
  else if (response[0] == '{')
    {
    // Other operations return a json description of an object.
    // E.g. GET query of the list of subjects
    qDebug() << "HELLO 3";
    result = d->parseJsonResponse(restResult, response);
    qDebug() << "result.size():" << result.size();
    }
  else if (identifierPattern.exactMatch(response))
    {
    // Some operations return the identifier of the newly created object.
    // E.g. creating a subject.
    QVariantMap map;
    map["ID"] = response;
    result.push_back(map);
    qDebug() << "HELLO 4";
    }
  else
    {
    qDebug() << "HELLO 5";
    restResult->setError(QString("Bad data: ") + response);
    }

  qDebug() << "result.size():" << result.size();
  foreach (QVariantMap map, result)
  {
    QMapIterator<QString, QVariant> it(map);
    for (it.next(); it.hasNext(); it.next())
    {
      qDebug() << it.key() << ":" << it.value();
    }
  }
  restResult->setResult(result);
}
