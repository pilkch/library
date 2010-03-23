#ifndef CDATABASE_H
#define CDATABASE_H

#ifndef FIRESTARTER
#include <spitfire/util/cString.h>
#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cColour.h>
#endif

namespace spitfire
{
  namespace database
  {
    // Use either the sqlite, mysql or couchdb API

    enum class DRIVER {
#ifdef BUILD_DATABASE_SQLITE
      SQLITE,
#endif
#ifdef BUILD_DATABASE_MYSQL
      MYSQL,
#endif
#ifdef BUILD_DATABASE_COUCHDB
      COUCHDB,
#endif

#ifdef BUILD_DATABASE_SQLITE
      DEFAULT = SQLITE
#elif defined(BUILD_DATABASE_MYSQL)
      DEFAULT = MYSQL
#else
      DEFAULT = COUCHDB
#endif
    };

    // Forward declaration
    class cDatabase;
    class cParameters;
    class cQuery;

    cDatabase* Create(DRIVER driver, const cParameters& parameters);
    void Destroy(cDatabase* pDatabase);


    class cDatabaseConnection;

    class cDatabaseReadOnly
    {
    public:
      cDatabaseReadOnly(const string_t& sHostName, const string_t& sUserName, const string_t& sPassword, const string_t& sDatabaseName);

      bool IsOpen() const;

      bool Select(const cQuery& query, cDatabaseResult& result);

    private:
      cDatabaseConnection* pConnection;
    };

    class cDatabaseWriteOnly
    {
    public:
      cDatabaseWriteOnly(const string_t& sHostName, const string_t& sUserName, const string_t& sPassword, const string_t& sDatabaseName);

      bool IsOpen() const;

      bool Insert(const cQuery& query);
      bool Update(const cQuery& query);

    private:
      cDatabaseConnection* pConnection;
    };

    class cDatabase
    {
    public:
      explicit cDatabase(const cParameters& parameters);
      virtual ~cDatabase() {}

      bool Open(const cParameters& parameters);
      bool Close();

      bool IsOpen() const;

      ...

    private:
      virtual bool _Open(const cParameters& parameters) = 0;
      virtual bool _Close() = 0;

      virtual bool _IsOpen() const = 0;

      ...
    };



    class cParameters
    {
    public:
      friend class cDatabase;

      cParameters();

      void SetHostName(const string_t& _sHostName) { sHostName = _sHostName; }
      void SetPort(port_t _port) { port = _port; }
      void SetUserName(const string_t& _sUserName) { sUserName = _sUserName; }
      void SetPassword(const string_t& _sPassword) { sPassword = _sPassword; }
      void SetDatabase(const string_t& _sDatabase) { sDatabase = _sDatabase; }

    private:
      string_t sHostName;
      port_t port;
      string_t sUserName;
      string_t sPassword;
      string_t sDatabase;
    };


    class cQuery
    {
    public:
      void SetTable(const string_t& sTableName);
      void SetWhere(const string_t& sWhere);
      void SetLimit(size_t limit);
      void SetAscending();

    private:
      bool bIsAscending;
    };


    class cResult
    {
    public:
      size_t GetNumberOfRows() const;

      bool GetString(size_t row, const string_t& sColumnName, string_t& sValue) const;
      bool GetUint64(size_t row, const string_t& sColumnName, uint64_t& sValue) const;
    };
  }
}

#endif // CDATABASE_H
