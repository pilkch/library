#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>

// Writing to and from text files
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

#include <spitfire/storage/database.h>

namespace spitfire
{
  namespace database
  {
    cParameters::cParameters() :
      port(0)
    {
    }


    class cDatabaseConnection
    {
    public:
      cDatabaseConnection(const string_t& sHostName, const string_t& sUserName, const string_t& sPassword, const string_t& sDatabaseName);

      bool IsOpen() const;

      bool Select(const cQuery& query, cDatabaseResult& result);
      bool Insert(const cQuery& query);
      bool Update(const cQuery& query);
    };


    // Use either the mysql, sqlite or couchdb API

#ifdef BUILD_SUPPORT_MYSQL

#elif defined(BUILD_SUPPORT_SQLITE)

#elif defined(BUILD_SUPPORT_COUCHDB)


#include <libcouchdb.h>

int process_list ( void *data, void *data_user );
int process_list_content ( void *data, void *data_user );
int process_list_content_field ( void *data, void *data_user );

int main ( void )
{
   Couch *c = NULL;
   CouchDocument *doc = NULL;
   List *l = NULL;

   c = couch_init ( "mycouchserver.mynetwork.net:5984", "mydb" );

   /* Tables are just special documents, but don't forget to give them uniue names! */
   doc = couch_doc_create ( "tabletest" );

   /* Set the table statement. You can use the fabric language for that.
   * Note that every field has to start with $table_ !
   */
   couch_doc_set ( doc, "$table_all", "SELECT *;", NULL );
   couch_doc_set ( doc, "$table_private", "SELECT Access=Joe; COLUMN Subject;", NULL );

   /* Save the table. */
   couch_doc_save ( c, doc );

   /* now use the table to query the database */
   l = couch_table_compute ( c, "tabletest:private" );

   /* okay we have now a list with the result. in the most cases, we want to go
   * through the list and look at it's entries. This is the best way to do that
   * (though others are possible).
   */
   list_foreach ( l, process_list, NULL );

   /* we're done, cleanup plz! */
   couch_list_cleanup ( &l );
   couch_doc_cleanup ( &doc );
   couch_cleanup ( &c );
}

/* return values might be:
 * -1: error -> terminate the loop
 *  0: don't jump to the next item -> process this item again
 *  1: everything is fine, go on
 *
 * parameters:
 *   data - pointer to the current item
 *   data_user - pointer to the data of the last parameter of list_foreach().
 *               this data is always the same for each item - unless you change it
 *               in this function of course ;-)
 */
int process_list ( void *data, void *data_user )
{
   CouchDocument *doc = NULL;

   if ( !data || data_user ) return -1;

   /* the list don't know which item it holds, so we need to cast them.
   * in the case of couch_table_compute(), we'll get a list of documents and their
   * contents.
   */
   doc = (CouchDocument *)data;

   /* now we can do cool things with the document. what about just print them to the
   * screen? ;-)
   */
   printf ( "Document: %s\n", doc->docid );

   /* what about the document fields? */
   list_foreach ( doc->content, process_list_content, NULL );

   /* everything is fine, go ahead */
   return 1;
}

int process_list_content ( void *data, void *data_user )
{
   CouchDocumentField *f = NULL;

   if ( !data || data_user ) return -1;

   /* now the items are CouchDocumentFields. */
   f = (CouchDocumentField *)data;

   /* print the name of the field. */
   printf ( "  Field: %s\n", f->name );

   /* hey, what about the values of a field? yes, third loop ;-) */
   list_foreach ( f->values, process_list_content_field, NULL );

   /* go go! */
   return 1;
}

int process_list_content_field ( void *data, void *data_user )
{
   CouchDocumentFieldContent *c = NULL,

   if ( !data || data_user )
          return -1;

   /* see above ;-) */
   c = (CouchDocumentFieldContent *)data;

   /* print the values */
   printf ( "      Content: %s\n", c->value );

   /* what about... no, forget that ;-) */



   /* go to the next item */
   return 1;
}

#endif

  }
}

void TestThisDatabase(spitfire::database::DRIVER driver, spitfire::port_t port)
{
  spitfire::database::cParameters parameters;
  parameters.SetHostName(TEXT("localhost"));
  parameters.SetPort(port);
  parameters.SetUserName(TEXT("user"));
  parameters.SetPassword(TEXT("password"));
  parameters.SetDatabase(TEXT("database"));

  spitfire::database::cDatabase* pDatabase = spitfire::database::Create(driver, parameters);
  ASSERT(pDatabase->IsOpen());

  spitfire::database::cQuery query;
  query.SetTable(TEXT("table"));
  pDatabase->Select(query);

  database::Destroy(pDatabase);
  pDatabase = nullptr;
}

void DatabaseUnitTest()
{
#ifdef BUILD_SUPPORT_SQLITE
  TestThisDatabase(spitfire::database::DRIVER::SQLITE, 10003);
#endif

#ifdef BUILD_SUPPORT_MYSQL
  TestThisDatabase(spitfire::database::DRIVER::MYSQL, 10003);
#endif

#ifdef BUILD_SUPPORT_COUCHDB
  TestThisDatabase(spitfire::database::DRIVER::COUCHDB, 10003);
#endif
}
