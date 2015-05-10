// AudioX.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "audiox_engine.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <iomanip>
#include "Timing.hpp"
#include "Downloader.h"
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace AudioX;

	std::ios_base::sync_with_stdio(false);

	std::streamsize totalBytes = 0;
	std::shared_ptr<Stream> stream = std::make_shared<Stream>();
	Downloader dl(stream);
	const std::streamsize maxDl = 10000000; // 30mb max download
	const std::streamsize chunkSize = 200000; // query for 200kb chunksize
	{
		Timing t("First DL 1MB");
		dl.newRequest("http://demo.cf.castlabs.com/media/TOS/DASH/tearsofsteel_4k.mov_856x380_1100.mp4");
		std::this_thread::sleep_for(std::chrono::seconds(4));
		int last = 0;
		while (stream->waitReadyRead(chunkSize) && totalBytes <= maxDl) {
			std::vector<char> buf;
			stream->read(buf);
			std::cout << buf.size() << "\n";
			totalBytes += buf.size();
		}
		{
			Timing t("Time to cancel");
			dl.cancel();
		}
	}
	std::cout << "\n";
	stream->streamInfo().print();

	std::cout << "\n\n\n";
	totalBytes = 0;
	{
		Timing t("Second DL 1MB");
		//dl.newRequest("http://demo.cf.castlabs.com/media/TOS/DASH/tearsofsteel_4k.mov_856x380_1100.mp4");
		dl.newRequest("http://ftp.rz.tu-bs.de/pub/mirror/knoppix/knoppix-dvd/KNOPPIX_V7.4.1DVD-2014-09-15-DE.iso");
		while (stream->waitReadyRead(chunkSize) && totalBytes <= maxDl) {
			std::vector<char> buf;
			stream->read(buf);
			std::cout << ".";
			totalBytes += buf.size();
		}
		std::cout << "\n";
		{
			Timing t("Time to cancel");
			dl.cancel();
		}
	}
	stream->streamInfo().print();
	std::cout << "Done!\n";
	return 0;
}

