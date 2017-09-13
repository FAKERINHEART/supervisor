#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "../../../share/api/ConfigManager.h"
#include "../../../rpc-thrift/src_gen/gen-cpp/MTServer.h"
#include "../../../share/lib/IpUtil.h"
#include "../../../share/fw/thrift/ThriftServiceClient.hpp"
#include "../../../rpc-thrift/src_gen/gen-cpp/exception_types.h"
#include "../../../share/program/extend_helper_v2.h"

using namespace service::mt;


void sendmail(std::string& _tstr, struct timeval& _tv, std::vector<std::string>& vector_std_in_content, std::fstream& f)
{
	
	try
	{
		const config::SocketServer& socketConf = config::ConfigManager::Instance().getServerConfig("mt_server");		
		std::string localIp = IpUtil::getLocalIP();
		std::map<std::string, std::string> arg;
		arg["subject"] = "[supervisord进程]告警";
		arg["content"] = "事件发生时间: " + _tstr + "." + boost::lexical_cast<std::string>(_tv.tv_usec) + "<br>";
		arg["content"] += "服务器IP: " + localIp + "<br>";
		for(int i = 0; i < vector_std_in_content.size(); ++i)
		{
			arg["content"] += vector_std_in_content[i] + "<br>";
		}
		
		ThriftServiceClient<MTServerClient> mt_server(socketConf.host, socketConf.port);
		
		SendNotifyResponse resp;
		SendNotifyRequest req;
		req.mail = "137328237@qq.com";
		req.notifyType = NotifyType::DIY;
		req.params = arg;
		req.svrIp = localIp;
		req.reqTime = _tv.tv_sec;
			
		mt_server.client.sendNotify(resp, req);
	}
	catch(BaseException& e)
	{
		f << "Failed to send the notify mail!" << std::endl;
	}
	catch(TException &tx)
	{
		f << "Failed to get IP or connect to mt_server!" <<  " " << tx.what() << std::endl;
	}
	return;
}

int main()
{
	std::map<std::string, std::string> en2cn{{"ver", "进程管理版本"}, {"server", "进程管理服务名称"}, {"serial", "事件序列号"}, {"pool", "线程池名称"}, {"poolserial", "线程序列号"}, {"eventname", "事件名称"}, {"processname", "进程名称"}, {"groupname", "进程组名称"}, {"from_state", "未发生事件前进程的状态"}, {"pid", "进程号"}};
	std::map<std::string, std::string> kv;
	std::string string_std_in;
	std::vector<std::string> vector_std_in_content;
	std::string string_std_in_content;
	std::string string_email_content;
	
	struct timeval _tv;
	struct tm *_tm;
	char _tbuff[128];
	std::string _tstr;

	while(true)
	{
		std::cout << "READY\n";
		
		
		gettimeofday(&_tv, NULL);
		_tm = localtime(&(_tv.tv_sec));
		strftime(_tbuff, 64, "%Y-%m-%d %H:%M:%S", _tm);
		_tstr = _tbuff;

		
		std::getline(std::cin, string_std_in, '\n');
		string_std_in_content = string_std_in;
	
        kv.clear();
		std::vector<std::string> vector_std_in;	
		boost::split(vector_std_in, string_std_in, boost::is_any_of(" "), boost::token_compress_on);	
		for(int i = 0; i < vector_std_in.size(); ++i)
		{
			std::vector<std::string> temp;
			boost::split(temp, vector_std_in[i], boost::is_any_of(":"), boost::token_compress_on);
			size_t sizeTemp = temp.size();
            if(sizeTemp == 2)
            {
                kv[temp[0]] = temp[1];
            }
            else if(sizeTemp == 1)
            {
                kv[temp[0]] = "";   
            }
		}
		
	    if(kv.find("len") == kv.end())
        {
		    std::cout << "RESULT 2\nOK";
            continue; 
        }

		int length = boost::lexical_cast<int>(kv["len"]);
		boost::shared_ptr<char[]> char_std_in(new char[length + 3]);
		std::cin.read(char_std_in.get(), length);
		char_std_in.get()[length + 1] = '\0';
		string_std_in = char_std_in.get();
		string_std_in_content += " ";
		string_std_in_content += string_std_in;
		
		boost::split(vector_std_in, string_std_in, boost::is_any_of(" "), boost::token_compress_on);	
		for(int i = 0; i < vector_std_in.size(); ++i)
		{
			std::vector<std::string> temp;
			boost::split(temp, vector_std_in[i], boost::is_any_of(":"), boost::token_compress_on);
			size_t sizeTemp = temp.size();
            if(sizeTemp == 2)
            {
                kv[temp[0]] = temp[1];
            }
            else if(sizeTemp == 1)
            {
                kv[temp[0]] == "";
            }
		}

		std::fstream f("/data/logs/server/supvr_event_callback/supvr_event_callback.log", std::fstream::in | std::fstream::out | std::fstream::app);
		f << _tstr << "." << _tv.tv_usec << " ";
		f << string_std_in_content << std::endl;

        
        bool is_PROCESS_STATE_STOPPED = false;//发生的是否是子进程STOPPED事件
		bool is_supvr_event_callback = false;//是否是子进程supvr_event_callback.bin发生了事件
        vector_std_in_content.clear();

		if(kv.find("processname") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["processname"] + ":" + kv["processname"]);
            if(kv["processname"] == "supvr_event_callback.bin")
            {
                is_supvr_event_callback == true;
            }
		}

		if(kv.find("pid") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["pid"] + ":" + kv["pid"]);
		}

		if(kv.find("groupname") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["groupname"] + ":" + kv["groupname"]);
		}

		if(kv.find("eventname") != kv.end())
		{	
			vector_std_in_content.push_back(en2cn["eventname"] + ":" + kv["eventname"]);
            if(kv["eventname"] == "PROCESS_STATE_STOPPED")
            {
                is_PROCESS_STATE_STOPPED = true;
            }
		}

		if(kv.find("from_state") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["from_state"] + ":" + kv["from_state"]);
		}

		if(kv.find("server") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["server"] + ":" + kv["server"]);
		}

		if(kv.find("ver") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["ver"] + ":" + kv["ver"]);
		}

		if(kv.find("serial") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["serial"] + ":" + kv["serial"]);
		}
		
		if(kv.find("pool") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["pool"] + ":" + kv["pool"]);
		}

		if(kv.find("poolserial") != kv.end())
		{
			vector_std_in_content.push_back(en2cn["poolserial"] + ":" + kv["poolserial"]);
		}
        
        if(is_PROCESS_STATE_STOPPED && is_supvr_event_callback || !is_PROCESS_STATE_STOPPED)
        {
            sendmail(_tstr, _tv, vector_std_in_content, f);
        }
		
		f << std::endl << std::endl;
		f.close();


		std::cout << "RESULT 2\nOK";
	}	


	return 0;
}
