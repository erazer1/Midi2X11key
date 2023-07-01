#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <fstream>
#include <memory>

// exterrnal dependencies which need to be installed
extern "C" {
#include <xdo.h>
}
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>
#include "RtMidi.h"

class Xdo
{
public:
	Xdo()
	{
		context = xdo_new(NULL);
	}
	~Xdo()
	{
		xdo_free(context);
	}
	void send_key(std::string const & sequence, useconds_t delay = 12000)
	{
		xdo_send_keysequence_window(context, CURRENTWINDOW, sequence.c_str(), delay);
	}
private:
	xdo_t* context;
};


using msgvec = std::vector<unsigned char>;
using keyspec = std::string;

using msgmapvec = std::map<msgvec, keyspec>;

struct progOpts
{
	unsigned int portNum;
	msgmapvec msgMap;
};

int count = 0;
Xdo xdomain;

std::ostream& operator<<(std::ostream& os, msgvec const &msgvec) 
{ 
	unsigned int nBytes = msgvec.size();
	os << "[ ";
	for ( unsigned int i=0; i<nBytes; i++ )
	{
		os << (int)msgvec.at(i);
		if(i<(nBytes-1))
		{
			os << ", ";
		}
	}
	os << " ]";
	
	return os;
}

void processMidiMessage( double deltatime, msgvec* message, void *userData )
{
	count++;
	msgmapvec* mapPtr = (msgmapvec*)userData;
	
	
	std::cout << *message;
	
	//std::cout << std::endl;
	//if ( nBytes > 0 )
	//	std::cout << "stamp = " << deltatime << std::endl;
		
	auto mapIter = mapPtr->find(*message);
	if(mapIter != mapPtr->end())
	{
		std::cout << " - " << mapIter->second << "\n";
		xdomain.send_key(mapIter->second);
	}
}

progOpts parseOptions()
{
	progOpts options;
	
	using json = nlohmann::json;

	std::ifstream conf("config.json");
	json data = json::parse(conf);
	
	options.portNum = data.at("port_in").get<unsigned int>();
	
	auto messageMapJson = data.at("message_map");
	
	options.msgMap = messageMapJson.get<msgmapvec>();
	
	return options;
	// use threads for listening
	// reading options - mainly exit and list
}

void receiveMidi(progOpts & options, std::atomic_bool& stop)
{
	//msgmapvec map = { {{196, 0}, "Left"}, {{196, 1}, "Right"} };
	
	std::unique_ptr<RtMidiIn> midiin;
	
	// RtMidiIn constructor
	try 
	{
		midiin.reset(new RtMidiIn());
	}
	catch ( RtMidiError &error ) 
	{
		error.printMessage();
		exit( EXIT_FAILURE );
	}
	
	// Check available ports.
	unsigned int nPorts = midiin->getPortCount();
	if ( nPorts > 0 ) 
	{
		midiin->openPort( options.portNum );
		// Set our callback function.  This should be done immediately after
		// opening the port to avoid having incoming messages written to the
		// queue.
		midiin->setCallback( &processMidiMessage, &options.msgMap );
		// Don't ignore sysex, timing, or active sensing messages.
		midiin->ignoreTypes( true, true, true );
		std::cout << "\nReading MIDI input ... receiving three messages\n";
		//char input;
		//std::cin.get(input);
		while(!stop)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	else
	{
		std::cout << "No ports available!\n";
	}
}

void midi_launch(std::atomic_bool& stop)
{
	auto options = parseOptions();
	receiveMidi(options, stop);
}

void enumeratePors()
{
	std::unique_ptr<RtMidiIn> midiin;
	std::unique_ptr<RtMidiOut> midiout;
	
	// RtMidiIn constructor
	try 
	{
		midiin.reset(new RtMidiIn());
	}
	catch ( RtMidiError &error ) 
	{
		error.printMessage();
		exit( EXIT_FAILURE );
	}

	// Check inputs.
	unsigned int nPorts = midiin->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";

	std::string portName;
 
	for ( unsigned int i=0; i<nPorts; i++ ) 
	{
		try 
		{
			portName = midiin->getPortName(i);
		}
		catch ( RtMidiError &error ) 
		{
			error.printMessage();
		}
		std::cout << "  Input Port #" << i << ": " << portName << '\n';
	}
	// RtMidiOut constructor
	try 
	{
		midiout.reset(new RtMidiOut());
	}
	catch ( RtMidiError &error ) 
	{
		error.printMessage();
		exit( EXIT_FAILURE );
	}
	
	// Check outputs.
	nPorts = midiout->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
  
	for ( unsigned int i=0; i<nPorts; i++ ) 
	{
		try 
		{
			portName = midiout->getPortName(i);
		}
		catch (RtMidiError &error) 
		{
			error.printMessage();
		}
		std::cout << "  Output Port #" << i << ": " << portName << '\n';
	}
	std::cout << '\n';
}

int main(int argc, char* argv[])
{
	//sendKeys();
	//enumeratePors();
	
	cxxopts::Options options("MidiKey", "Forwards arbitrary midi messages to keys");

	options.add_options()
		("p,ports", "List ports")
		("f,forward", "Forward messages to keys")
		;

	cxxopts::ParseResult result;
	
	try
	{
		result = options.parse(argc, argv);
	}
	catch (cxxopts::exceptions::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}

	if (argc == 1)
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}

	if (result.count("ports"))
	{
		enumeratePors();
		exit(0);
	}

	if (result.count("forward"))
	{
		std::atomic_bool stop = { false };

		std::cout << "Press enter to stop...\n";
		
		auto future = std::async(std::launch::async, midi_launch, std::ref(stop));

		std::cin.get();

		stop = true;

		future.get();

		exit(0);
	}
	
}
