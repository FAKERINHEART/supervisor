#include <dirent.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <mstch/mstch.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

std::string input_path          = "../../../rpc-thrift/idl";
std::string output_path         = "../supvr.conf.d";
std::string prefix_command_path = "/data/release/server/dist";
std::map<std::string, std::string> absolute_file_bin_path;

void eventlistener_template2conf()
{
	DIR* dp;
	struct dirent* dirp;
	dp = opendir(output_path.c_str());
	int count_file = 0;
	while (dirp = readdir(dp))
	{
		if (dirp->d_type == DT_REG)
		{
			++count_file;
		}
	}
	closedir(dp);

	std::fstream f("supervisor_enslaved_eventlistener_template.supvr.conf", std::fstream::in);
	std::istreambuf_iterator<char> beg(f), end;
	std::string string_file(beg, end);
	f.close();

	mstch::map ctx = {{"num_procs", count_file}, {"buffer_size", count_file * 2}};
	std::string supvisord_enslaved_eventlistener_conf = mstch::render(string_file, ctx);

	std::string ouput_absolute_file_name = output_path + "/" + "supvr_event_callback.bin.supvr.conf";
	f.open(ouput_absolute_file_name, std::fstream::in | std::fstream::out | std::fstream::trunc);
	f << supvisord_enslaved_eventlistener_conf << '\n';
	f.close();

	return;
}

void recursive_traverse_bin_file(std::string path)
{
	DIR* dp;
	struct dirent* dirp;
	dp = opendir(path.c_str());
	std::string temp_file_bin_name;

	while (dirp = readdir(dp))
	{
		temp_file_bin_name = dirp->d_name;
		if (dirp->d_type == DT_REG || dirp->d_type == DT_LNK)
		{
			absolute_file_bin_path[temp_file_bin_name] = path;
		}
		else if (dirp->d_type == DT_DIR)
		{
			if (temp_file_bin_name != "." && temp_file_bin_name != "..")
			{
				recursive_traverse_bin_file(path + "/" + temp_file_bin_name);
			}
		}
	}
	closedir(dp);

	return;
}

void service_template2conf(std::string input_absolute_file_name)
{
	std::fstream f(input_absolute_file_name, std::fstream::in);
	std::istreambuf_iterator<char> beg(f), end;
	std::string string_file(beg, end);
	f.close();

	boost::regex reg_erase_double_slash("//[\\x00-\\xff]*?\n");
	string_file = boost::regex_replace(string_file, reg_erase_double_slash, "");
	boost::regex reg_erase_asterisk_slash("/\\*[\\x00-\\xff]*?\\*/");
	string_file = boost::regex_replace(string_file, reg_erase_asterisk_slash, "");

	boost::regex reg(
	    "service[ \t\n]+[A-Za-z0-9_]*[ \t\n]+(extends [A-Za-z0-9_\\.]+[ \t\n]*)?\{[\\x00-\\xff]*?}[ \t\n]*\\([ \t\n]*port[ \t\n]*=[ \t\n]*\"[1-9]+[0-9]*\"\\)");
	boost::sregex_iterator siter(string_file.begin(), string_file.end(), reg);
	boost::sregex_iterator siter_end;

	boost::regex reg2("port[ \t\n]*=[ \t\n]*\"[1-9]+[0-9]*\"");
	boost::smatch sma2;

	boost::regex reg3("[1-9]+[0-9]*");
	boost::smatch sma3;

	std::string service_frame, service_name, port_name;
	int port_start, port_end;
	std::stringstream ss;
	while (siter != siter_end)
	{
		service_frame = siter->str();
		ss.str(service_frame);
		ss >> service_name >> service_name;
		std::string temp;
		for (int i = 0; i < service_name.size(); ++i)
		{
			if (service_name[i] >= 'A' && service_name[i] <= 'Z')
			{
				if (i)
					temp.append(1, '_');
				temp.append(1, service_name[i] + 'a' - 'A');
			}
			else
			{
				temp.append(1, service_name[i]);
			}
		}
		service_name = temp;

		boost::regex_search(service_frame, sma2, reg2);
		port_name = sma2[0];
		boost::regex_search(port_name, sma3, reg3);
		port_name = sma3[0];

		if (absolute_file_bin_path.find(service_name + ".bin") != absolute_file_bin_path.end())
		{
			std::fstream f1("supervisor_enslaved_service_template.supvr.conf", std::fstream::in);
			std::istreambuf_iterator<char> beg1(f1), end1;
			std::string string_file1(beg1, end1);
			f1.close();

			mstch::map ctx = {{"service_name", service_name},
			                  {"port_name", port_name},
			                  {"absolute_file_bin_name", absolute_file_bin_path[service_name + ".bin"] + "/" + service_name + ".bin"},
			                  {"absolute_file_bin_path", absolute_file_bin_path[service_name + ".bin"]}};
			std::string supvisord_enslaved_service_conf = mstch::render(string_file1, ctx);

			std::string ouput_absolute_file_name = output_path + "/" + service_name + ".bin.supvr.conf";
			f.open(ouput_absolute_file_name, std::fstream::in | std::fstream::out | std::fstream::trunc);
			f << supvisord_enslaved_service_conf << '\n';
			f.close();
		}

		++siter;
	}
	return;
}

void recursive_traverse_idl_file(std::string path)
{
	DIR* dp_in;
	struct dirent* dirp;
	dp_in = opendir(path.c_str());
	std::string temp_file_name;
	while (dirp = readdir(dp_in))
	{
		temp_file_name = dirp->d_name;
		if (dirp->d_type == DT_REG)
		{
			if (temp_file_name.find_last_of(".thrift") == temp_file_name.length() - 1 && temp_file_name.length() >= 8)
			{
				service_template2conf(path + "/" + temp_file_name);
			}
		}
		else if (dirp->d_type == DT_DIR)
		{
			if (temp_file_name != "." && temp_file_name != "..")
			{
				recursive_traverse_idl_file(path + "/" + temp_file_name);
			}
		}
	}
	closedir(dp_in);

	return;
}

int main(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description po_desc;
	po_desc.add_options()("help,h", "Print this help list")("input-path,i", po::value<std::string>(&input_path)->default_value("../../../rpc-thrift/idl"),
	                                                        "input idl path, default(../../../rpc-thrift/idl)")(
	    "output-path,o", po::value<std::string>(&output_path)->default_value("../supvr.conf.d"), "output configure path, default(../supvr.conf.d)")(
	    "prefix command path,p", po::value<std::string>(&prefix_command_path)->default_value("/data/release/server/dist"),
	    "prefix_command_path, default(/data/release/server/dist)");
	po::variables_map po_vm;
	po::store(po::command_line_parser(argc, argv).options(po_desc).style(0).run(), po_vm);
	po::notify(po_vm);
	if (po_vm.count("help"))
	{
		std::cout << po_desc << std::endl;
		exit(0);
	}

	DIR *dp_in, *dp_out;
	struct dirent* dirp;

	if ((dp_in = opendir(input_path.c_str())) == NULL)
	{
		std::cout << "Your input path: " + input_path + " does not exist!" << std::endl;
		exit(1);
	}
	if ((dp_out = opendir(output_path.c_str())) == NULL)
	{
		if (mkdir(output_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1)
		{
			std::cout << "Your output path: " + output_path + "does not exist, and you are not permitted to create this folder." << std::endl;
			exit(1);
		}
	}

	recursive_traverse_bin_file(prefix_command_path);
	recursive_traverse_idl_file(input_path);

	eventlistener_template2conf();

	return 0;
}
