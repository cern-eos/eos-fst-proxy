#ifndef __XRDFILECACHE_FACTORY_HH__
#define __XRDFILECACHE_FACTORY_HH__
//----------------------------------------------------------------------------------
// Copyright (c) 2014 by Board of Trustees of the Leland Stanford, Jr., University
// Author: Alja Mrak-Tadel, Matevz Tadel, Brian Bockelman
//----------------------------------------------------------------------------------
// XRootD is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// XRootD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with XRootD.  If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------------

#include <string>
#include <vector>

#include <XrdSys/XrdSysPthread.hh>
#include <XrdOuc/XrdOucCache.hh>
#include "XrdVersion.hh"
class XrdOucStream;
class XrdSysError;

#include "XrdFileCacheDecision.hh"
#include "XrdFileCacheLog.hh"

namespace XrdFileCache
{
   //----------------------------------------------------------------------------
   //! Contains all configurable parameters parsed by Factory class.
   //----------------------------------------------------------------------------
   struct Configuration
   {
      Configuration() :
         m_prefetchFileBlocks(false),
         m_cache_dir("/var/tmp/xrootd-file-cache"),
         m_username("nobody"),
         m_lwm(0.95),
         m_hwm(0.9),
         m_logLevel(kError),
         m_bufferSize(1024*1024),
         m_blockSize(128*1024*1024) {}

      bool m_prefetchFileBlocks;      //!< flag to enable file block prefetch 
      std::string m_cache_dir;        //!< directory path to disk cache
      std::string m_username;         //!< username used for instantiation of oss plugin
      std::string m_osslib_name;      //!< oss library path
      float m_lwm;                    //!< cache purge low water mark 
      float m_hwm;                    //!< cache purge high water mark

      LogLevel  m_logLevel;           //!< file cache log level (0->dump, 1->debug, 2->info ...)
      long long m_bufferSize;         //!< prefetch buffer size, default 1MB 
      long long m_blockSize;          //!< used with m_prefetchFileBlocks, default 128MB
   };


   //----------------------------------------------------------------------------
   //! Instantiates caching and decision plugin. Parsers configuration file.
   //----------------------------------------------------------------------------
   class Factory : public XrdOucCache
   {
      public:
         //--------------------------------------------------------------------------
         //! Constructor
         //--------------------------------------------------------------------------
         Factory();

         //---------------------------------------------------------------------
         //! \brief Attached  Do nothing -- this is not a cache object.
         //!             
         //! return    Null
         //---------------------------------------------------------------------
         virtual XrdOucCacheIO *Attach(XrdOucCacheIO *, int Options=0){ return NULL; }

         //---------------------------------------------------------------------
         //! \brief isAttached  This cache instance can be delete. Need this method to
         //!             check if there are any associated objects. 
         //! return      No object attached.
         //---------------------------------------------------------------------
         virtual int isAttached() { return false; }
    
         //---------------------------------------------------------------------
         //! Create Creates XrdFileCache::Cache object
         //---------------------------------------------------------------------
         virtual XrdOucCache *Create(Parms &, XrdOucCacheIO::aprParms *aprP);

         XrdOss*GetOss() const { return m_output_fs; }

         //---------------------------------------------------------------------
         //! GetSysError Getter for xrootd logger
         //---------------------------------------------------------------------        
         XrdSysError&GetSysError() { return m_log; }

         //--------------------------------------------------------------------
         //! \brief Decide checks decision plugins.
         //!
         //! @param & URL of file
         //!
         //! @return decision if OucIO will cache.
         //--------------------------------------------------------------------
         bool Decide(const char* path);
       
         //------------------------------------------------------------------------
         //! Refeference XrdFileCache configuration 
         //------------------------------------------------------------------------
         const Configuration& RefConfiguration() const { return m_configuration; }

         //---------------------------------------------------------------------
         //! Thread for cache purge
         //---------------------------------------------------------------------
         void TempDirCleanup();

         //---------------------------------------------------------------------
         //! Config parse configuration file
         //!
         //! @param logger             xrootd logger       
         //! @param config_filename    path to configuration file
         //! @param parameters         optional parameters to be passed
         //!
         //! @return parse status
         //---------------------------------------------------------------------
         bool Config(XrdSysLogger *logger, const char *config_filename, const char *parameters);

         //---------------------------------------------------------------------
         //! GetInstance Get this object
         //---------------------------------------------------------------------
         static Factory &GetInstance();

         //---------------------------------------------------------------------
         //! Support version check
         //---------------------------------------------------------------------
         static bool VCheck(XrdVersionInfo &urVersion) { return true; }

      private:
         bool ConfigParameters(const char *);
         bool ConfigXeq(char *, XrdOucStream &);
         bool xolib(XrdOucStream &);
         bool xdlib(XrdOucStream &);

         static XrdSysMutex m_factory_mutex;
         static Factory * m_factory;

         XrdSysError m_log;
         XrdOucCacheStats m_stats;
         XrdOss *m_output_fs;

         std::vector<XrdFileCache::Decision*> m_decisionpoints;

         Configuration m_configuration;
   };

}

#endif
