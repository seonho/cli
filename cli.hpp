/**
 *	@file		cli.hpp
 *	@brief		This is command line interpreter
 *	@author		seonho.oh@gmail.com
 *	@date		2014-02-05
 *	@version	1.0
 *
 *	@section	LICENSE
 *
 *		Copyright (c) 2007-2014, Seonho Oh
 *		All rights reserved. 
 * 
 *		Redistribution and use in source and binary forms, with or without  
 *		modification, are permitted provided that the following conditions are  
 *		met: 
 * 
 *		    * Redistributions of source code must retain the above copyright  
 *		    notice, this list of conditions and the following disclaimer. 
 *		    * Redistributions in binary form must reproduce the above copyright  
 *		    notice, this list of conditions and the following disclaimer in the  
 *		    documentation and/or other materials provided with the distribution. 
 *		    * Neither the name of the <ORGANIZATION> nor the names of its  
 *		    contributors may be used to endorse or promote products derived from  
 *		    this software without specific prior written permission. 
 * 
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS  
 *		IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  
 *		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  
 *		PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER  
 *		OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  
 *		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,  
 *		PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR  
 *		PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  
 *		LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  
 *		NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS  
 *		SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

template<typename T>
struct select_cout;

template <>
struct select_cout<char> { static std::ostream& cout; };

template <>
struct select_cout<wchar_t> { static std::wostream& cout; };

//! Forward declaration
template <typename _Elem>
class commandlineinterpreter;

//! Defines command interface
template <typename _Elem>
class command_base
{
	typedef _Elem							char_type; 
	typedef std::basic_string<char_type>	string;
	
public:
	//! The command
	virtual string	command() = 0;
	
	//! The number of arguments
	virtual int		argumentCount() = 0;
	
	//! The argument name at index
	virtual string	argumentName(int index) = 0;

	//! Run command
	virtual bool	run(commandlineinterpreter<_Elem>* pInterpreter) = 0;
protected:
	command_base() {}
};

//! Defines command line interpreter
template <typename _Elem>
class commandlineinterpreter
{
	typedef _Elem							char_type; 
	typedef std::basic_string<char_type>	string;
	typedef command_base<char_type> *		command_ptr;
public:
	//! ctor.
	commandlineinterpreter(int argc, char_type** argv): argc_(argc), argv_(argv)
	{
	}

	//! dtor.
	~commandlineinterpreter() { clear(); }

	//! Register command
	inline void register_command(command_ptr pCommand)	{ commands_.push_back(pCommand); }

	// process 
	int run()
	{
		if (argc_ < 2) {
			help();
			return 1;
		}

		int argc = argc_ - 2; // ignore filename and the command name
		string command = argv_[1];
		command_ptr pCommand = nullptr;
		auto itr = std::find_if(commands_.begin(), commands_.end(), [&](command_ptr c) {
			return c->command().compare(command) == 0;
		});

		if (itr != commands_.end()) pCommand = (*itr);

		if (pCommand == nullptr) {
			printf("Unknown command: %s\n", command.c_str());
			help();
			return 1;
		}

		if (pCommand->argumentCount() != argc) {
			printf("Wrong number of arguments for command: %s\n", command.c_str());
			help();
			return 1;
		}

		if (pCommand->run(this)) return 0;

		return 1;
	}

	template <typename T>
	inline T get(int index)
	{
		T value;
		std::basic_istringstream<char_type>(argv_[index + 2]) >> value;
		return value;
	}

	template <>
	inline int get(int index) { return std::stoi(argv_[index + 2]);	}

	template <>
	inline long get(int index) { return std::stol(argv_[index + 2]); }

	template <>
	inline unsigned long get(int index) { return std::stoul(argv_[index + 2]); }

	template <>
	inline unsigned long long get(int index) { return std::stoull(argv_[index + 2]); }
	
	template <>
	inline float get(int index) { return std::stof(argv_[index + 2]); }

	template <>
	inline double get(int index) { return std::stod(argv_[index + 2]); }

	template <>
	inline long double get(int index) { return std::stold(argv_[index + 2]); }

	inline void clear()
	{
		std::for_each(commands_.begin(), commands_.end(), [](command_ptr& c) {
			delete c;
			c = nullptr;
		});
	}

private:
	//! Print help
	void help()
	{
		std::basic_ostream<char_type>& cout = select_cout<char_type>::cout;
		cout << "built " << __DATE__ << " " << __TIME__ << std::endl << std::endl << "Commands:" << std::endl;
		std::for_each(commands_.begin(), commands_.end(), [&](command_ptr c) {
			cout << c->command() << " ";
			for (int i = 0 ; i < c->argumentCount() ; i++)
				cout << "<" << c->argumentName(i) << "> ";
			cout << std::endl;
		});
	}

	std::vector<command_ptr>	commands_;	//! the list of commands
	int							argc_;		//! the argument count
	char_type**					argv_;		//! the argument string
};

std::ostream& select_cout<char>::cout = std::cout;
std::wostream& select_cout<wchar_t>::cout = std::wcout;