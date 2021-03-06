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

#include "OCPlatform.h"
#include "OCApi.h"
#include "logger.h"

#include <functional>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace std;
using namespace OC;
namespace PH = std::placeholders;
std::mutex resourceLock;

OCResourceHandle resourceHandle;
shared_ptr< OCResource > g_resource;
vector< string > lights;
std::mutex blocker;
std::condition_variable cv;

bool isReady = false;

void onGet(const HeaderOptions& opt, const OCRepresentation &rep, const int eCode);

void onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode);

void onPost(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode);

void foundResource(std::shared_ptr< OCResource > resource)
{
    std::lock_guard<std::mutex> lock(resourceLock);
    if(g_resource)
    {
        std::cout << "Found another resource, ignoring"<<std::endl;
        return;
    }

    std::string resourceURI;
    std::string hostAddress;

    try
    {
        cout << "FOUND Resource" << endl;

        if (resource)
        {
            string resourceURI = resource->uri();
            cout << resourceURI << endl;
            cout << "HOST :: " << resource->host() << endl;
            if (resourceURI == "/core/a/collection")
            {
                g_resource = resource;
                resource->get("", DEFAULT_INTERFACE, QueryParamsMap(), onGet);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << "" << std::endl;
    }
}

void onGet(const HeaderOptions& opt, const OCRepresentation &rep, const int eCode)
{
    // printf("onGet\n");

    std::vector< OCRepresentation > children = rep.getChildren();

    cout << "\n\n\nCHILD RESOURCE OF GROUP" << endl;
    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        lights.push_back((*iter).getUri());
        cout << "\tURI :: " << (*iter).getUri() << endl;
    }

    isReady = true;
    cv.notify_one();
}

void onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    printf("\nonPut\n");
}

void onPost(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    printf("\nonPost\n");

    std::vector< OCRepresentation > children = rep.getChildren();

    cout << "\n\n\nCHILD RESOURCE OF GROUP" << endl;
    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        std::string power;
        (*iter).getValue("power", power);

        cout << "\tURI :: " << (*iter).getUri() << endl;
        cout << "\t\tpower :: " << power << endl;
    }

    if (rep.hasAttribute("ActionSet"))
    {
        std::string plainText;

        rep.getValue("ActionSet", plainText);

        printf("\tPlain Text :: %s\n", plainText.c_str());
    }
    else
    {
        printf("Not found ActionSet\n");
    }
}

string buildActionSetDesc()
{
    string actionsetDesc = "";
    //"movieTime*uri=coap://10.251.44.228:49858/a/light|power=10*uri=coap:
    //10.251.44.228:49858/a/light|power=10|";
    actionsetDesc = "allbulboff";
    actionsetDesc.append("*");
    for (auto iter = lights.begin(); iter != lights.end(); ++iter)
    {
        actionsetDesc.append("uri=").append((*iter));
        actionsetDesc.append("|");
        actionsetDesc.append("power=");
        actionsetDesc.append("off");
        if ((iter + 1) != lights.end())
        {
            actionsetDesc.append("*");
        }
    }
    return actionsetDesc;
}

bool isResourceReady()
{
    return isReady;
}
int main()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    bool isRun = true;

    try
    {
        OCPlatform::Configure(config);

        string resourceTypeName = "a.collection";
        OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=a.collection", &foundResource);

        //Non-intensive block util foundResource callback is called by OCPlatform
        //and onGet gets resource.
        //isResourceReady takes care of spurious wake-up

        std::unique_lock<std::mutex> lock(blocker);
        cv.wait(lock, isResourceReady);

        while (isRun)
        {
            usleep(100);
            int selectedMenu;

            cout << endl
                 << "0 :: Quit 1 :: CREATE ACTIONSET 2 :: EXECUTE ACTION SET \n";
            cout << "3 :: GET ACTIONSET 4 :: DELETE ACTIONSET \n" << endl;

            cin >> selectedMenu;

            OCRepresentation rep;
            string actionsetDesc;
            switch(selectedMenu)
            {
                case 0:
                    isRun = false;
                    break;
                case 1:
                    actionsetDesc = buildActionSetDesc();

                    if(!actionsetDesc.empty())
                    {
                        cout << "ActionSet :: " << actionsetDesc << endl;
                        rep.setValue("ActionSet", actionsetDesc);
                    }

                    if (g_resource)
                    {
                        g_resource->put("a.collection", GROUP_INTERFACE, rep, QueryParamsMap(),
                                &onPut);
                    }
                    break;
                case 2:
                    rep.setValue("DoAction", std::string("allbulboff"));
                    if (g_resource)
                    {
                        g_resource->post("a.collection", GROUP_INTERFACE, rep, QueryParamsMap(),
                            &onPost);
                    }
                    break;
                case 3:
                    rep.setValue("GetActionSet", std::string("allbulboff"));
                    if (g_resource)
                    {
                        g_resource->post("a.collection", GROUP_INTERFACE, rep, QueryParamsMap(),
                            &onPost);
                    }
                    break;
                case 4:
                    rep.setValue("DelActionSet", std::string("allbulboff"));
                    if (g_resource)
                    {
                        g_resource->put("a.collection", GROUP_INTERFACE, rep, QueryParamsMap(),
                            &onPut);
                    }
                    break;
                default:
                    cout << "Incorrect option" << endl;
                    break;

            }
            fflush(stdin);
        }
    }
    catch (OCException& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
