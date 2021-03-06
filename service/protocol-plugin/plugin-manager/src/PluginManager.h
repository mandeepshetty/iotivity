//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file PluginManager.h

/// @brief

#ifndef __PLUGINMANAGER_H
#define __PLUGINMANAGER_H
#define DLOPEN_POSIX

#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>
#include "PluginManagerImpl.h"
#include "Plugin.h"

namespace OIC
{

    /**
    * @brief    After installing a plug-in in a directory, each plug-ins can be managed by this class.
    *
    *
    */
    class PluginManager
    {
        public:
            PluginManager();

            /**
            * Virtual destructor
            */
            ~PluginManager(void);
            /**
            * Start  plugins by resource type
            *
            * @param type resouce type string to be started.
            *
            */
            int startPlugins(const std::string key, const std::string value);

            /**
            * Stop  plugins by resource type
            *
            * @param type resouce type string to be started.
            *
            */
            int stopPlugins(const std::string key, const std::string value);

            /**
            * Rescan Plugin.
            * This function will call rescan function of plugins in the configuration folder
            *
            * @param Plugin
            * @return int, 1 is success, 0 is fail.
            */
            int rescanPlugin();

            /**
            * Get Plugin list.
            *
            *
            *
            * @return
            */
            std::vector<Plugin> getPlugins(void);

            /**
            * Get Plugin state.
            *
            * @param Plugin ID
            * @return Plugin state.
            */
            std::string getState(const std::string plugID);

        private:
            PluginManagerImpl *pluginManagerImpl;
            void (*destroy)(PluginManagerImpl *);
            void *handle;
    };
}
#endif //__PLUGINMANAGER_H
