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

#ifndef __qXnatAPI_p_h
#define __qXnatAPI_p_h

// qXnatAPI includes
#include "qXnatAPI.h"

#include <QList>
#include <QScriptEngine>
#include <QScriptValue>

// --------------------------------------------------------------------------
class qXnatAPIPrivate : public QObject
{
  Q_DECLARE_PUBLIC(qXnatAPI);
  Q_OBJECT

  typedef QObject Superclass;

  qXnatAPI* const q_ptr;

  QScriptEngine ScriptEngine;

public:
  qXnatAPIPrivate(qXnatAPI* object);

  QList<QVariantMap> parseXmlResponse(qRestResult* restResult, const QByteArray& response);

  QList<QVariantMap> parseJsonResponse(qRestResult* restResult, const QByteArray& response);
};

#endif
