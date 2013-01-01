/*==============================================================================

  Library: qRestAPI

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

#ifndef __qRestAPI_h
#define __qRestAPI_h

// Qt includes
#include <QObject>
#include <QNetworkAccessManager>
#include <QScriptValue>
#include <QUuid>
#include <QVariant>

template <class Key, class T> class QMap;
typedef QMap<QString, QVariant> QVariantMap;

class QNetworkReply;
class qRestAPIPrivate;

class qRestResult;

/// qRestAPI is a simple interface class to communicate with web services
/// through a public RESTful API.
/// Requests are sent to the server and answers reported back.
/// qRestAPI works in synchronous or asynchronous way.
/// The class should be adopted to specific web services by subclassing. The
/// derived class should define how to construct the requests and how to
/// interpret the responses.
/// The library provides a sample implementation to interact with Midas and
/// XNAT servers.
/// Usage:
/// <code>
/// qRestAPI* midas = new qMidasAPI();
/// midas->setServerUrl("http://slicer.kitware.com/midas3");
/// connect(midas, SIGNAL(resultReceived(QUuid,QList<QVariantMap>)),
///         myApp, SLOT(processResult(QUuid,QList<QVariantMap>)));
/// midas->query("midas.version");
/// ...
/// delete midas;
/// </code>
class qRestAPI : public QObject
{
  Q_OBJECT

  /// URL of the web application. E.g. "http://slicer.kitware.com/midas3"
  Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl)

  /// Max time to wait until last progress of a query
  Q_PROPERTY(int timeOut READ timeOut WRITE setTimeOut)

  /// Default raw headers to be set for each requests. E.g. it can be used to set
  /// the user-agent or authentication information.
  Q_PROPERTY(RawHeaders defaultRawHeaders READ defaultRawHeaders WRITE setDefaultRawHeaders)

  /// Suppress SSL errors. Can be used to bypass self-signed certificates.
  Q_PROPERTY(bool suppressSslErrors READ suppressSslErrors WRITE setSuppressSslErrors)

  typedef QObject Superclass;

public:
  /// Constructs a qRestAPI object.
  explicit qRestAPI(QObject*parent = 0);
  /// Destructs a qRestAPI object.
  virtual ~qRestAPI();

  /// Type of the parameter list of a REST request.
  typedef QMap<QString, QString> Parameters;
  /// Type of the raw header list of a REST request.
  typedef QMap<QByteArray, QByteArray> RawHeaders;

  /// Returns the URL of the web application.
  QString serverUrl()const;
  /// Sets the URL of the web application.
  void setServerUrl(const QString& serverUrl);

  /// Returns the raw headers that are set for every request.
  RawHeaders defaultRawHeaders()const;
  /// Sets the raw headers to be set for every request.
  void setDefaultRawHeaders(const RawHeaders& defaultRawHeaders);

  /// Tells if the SSL errors are suppressed or not.
  bool suppressSslErrors()const;
  /// Sets if the SSL errors are to be suppressed or not.
  void setSuppressSslErrors(bool suppressSslErrors);

  void setTimeOut(int msecs);
  int timeOut()const;

  /// Sends a GET request to the web service.
  /// The \a resource and \parameters are used to compose the URL.
  /// \a rawHeaders can be used to set the raw headers of the request to send.
  /// These headers will be set additionally to those defined by the
  /// \a defaultRawHeaders property.
  /// errorReceived() is emitted if no server is found or if the server sends
  /// errors.
  /// resultReceived() is emitted when a result is received from the server,
  /// it is fired even if errors are received.
  /// Returns a unique identifier of the posted query.
  virtual QUuid get(const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  virtual QUuid get(QIODevice* output,
    const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  /// Downloads a file from the web service.
  /// \a fileName is the name of the output file.
  /// The \a resource and \parameters are used to compose the URL.
  /// \a rawHeaders can be used to set the raw headers of the request to send.
  /// These headers will be set additionally to those defined by the
  /// \a defaultRawHeaders property.
  /// errorReceived() is emitted if no server is found or if the server sends
  /// errors.
  /// resultReceived() is emitted when a result is received from the server,
  /// it is fired even if errors are received.
  /// Returns a unique identifier of the posted query.
  virtual QUuid download(const QString& fileName,
    const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  /// Sends a DELETE request to the web service.
  /// The \a resource and \parameters are used to compose the URL.
  /// \a rawHeaders can be used to set the raw headers of the request to send.
  /// These headers will be set additionally to those defined by the
  /// \a defaultRawHeaders property.
  /// errorReceived() is emitted if no server is found or if the server sends
  /// errors.
  /// resultReceived() is emitted when a result is received from the server,
  /// it is fired even if errors are received.
  /// Returns a unique identifier of the posted query.
  QUuid del(const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  /// Sends a POST request to the web service.
  /// The \a resource and \parameters are used to compose the URL.
  /// \a rawHeaders can be used to set the raw headers of the request to send.
  /// These headers will be set additionally to those defined by the
  /// \a defaultRawHeaders property.
  /// errorReceived() is emitted if no server is found or if the server sends
  /// errors.
  /// resultReceived() is emitted when a result is received from the server,
  /// it is fired even if errors are received.
  /// Returns a unique identifier of the posted query.
  QUuid post(const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  /// Sends a PUT request to the web service.
  /// The \a resource and \parameters are used to compose the URL.
  /// \a rawHeaders can be used to set the raw headers of the request to send.
  /// These headers will be set additionally to those defined by the
  /// \a defaultRawHeaders property.
  /// errorReceived() is emitted if no server is found or if the server sends
  /// errors.
  /// resultReceived() is emitted when a result is received from the server,
  /// it is fired even if errors are received.
  /// Returns a unique identifier of the posted query.
  QUuid put(const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  QUuid upload(const QString& fileName,
    const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

  bool sync(const QUuid& queryId);
  bool sync(const QUuid& queryId, QList<QVariantMap>& result);

  qRestResult* takeResult(const QUuid& queryId);

  /// Utility function that transforms a QList of QVariantMap into a string.
  /// Mostly for debug purpose.
  static QString qVariantMapListToString(const QList<QVariantMap>& variants);

signals:
  void finished(const QUuid& queryId);
  void progress(const QUuid& queryId, double progress);

protected:
  QNetworkReply* sendRequest(QNetworkAccessManager::Operation operation,
      const QUrl& url,
      const RawHeaders& rawHeaders = RawHeaders());

  virtual QUrl createUrl(const QString& method, const qRestAPI::Parameters& parameters);
  virtual void parseResponse(qRestResult* restResult, const QByteArray& response);

  static QString qVariantMapToString(const QList<QVariantMap>& result);
  static QVariantMap scriptValueToMap(const QScriptValue& value);
  static void appendScriptValueToVariantMapList(QList<QVariantMap>& result, const QScriptValue& value);

private:
  QScopedPointer<qRestAPIPrivate> d_ptr;

  Q_DECLARE_PRIVATE(qRestAPI);
  Q_DISABLE_COPY(qRestAPI);
};

#endif
