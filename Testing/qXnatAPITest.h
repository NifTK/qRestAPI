/*==============================================================================

  Program: qXnatAPI

  Copyright (c) 2013 Centre for Medical Image Computing, University College London

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qXnatAPITest_h
#define __qXnatAPITest_h

// Qt includes
#include <QObject>

#include "qXnatAPI.h"

class TestObject : public QObject
{
public:
  mutable QString id;
  mutable QString name;
  void operator=(const TestObject& other) const {
    id = other.id;
    name = other.name;
  }
};

class TestList : public QObject
{

};

class QXnatAPITest: public QObject
{
  Q_OBJECT

  void wait(int msec);

private slots:
  void initTestCase();

  void testReplaceChild();
  void testProjectList();
  void testProject();

  void testCreateSubject();
  void testDeleteSubject();

  void testDownloadScans();
  void testWaitFor();

  void cleanupTestCase();

private:
  qXnatAPI* xnat;
};

// --------------------------------------------------------------------------
int qXnatAPITest(int argc, char* argv[]);

#endif
