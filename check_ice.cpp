#include <Ice/Initialize.h>
#include <boost/program_options.hpp>
#include <chrono>
#include <compileTimeFormatter.h>

namespace po = boost::program_options;

AdHocFormatter(EndpointFmt, "%?:%? -h %? -p %?");
AdHocFormatter(Good, " OK: %? %?ms");
AdHocFormatter(Bad, " Bad: %? %?");

template<typename T>
T &
atLeast(T & v, const T & v2)
{
	return (v = std::max(v, v2));
}

template<typename F>
auto
stopwatch(const F & f)
{
	auto t_start = std::chrono::steady_clock::now();
	f();
	return std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - t_start).count();
}

int
main(int argc, char ** argv)
{
	std::string host, protocol;
	Ice::StringSeq objects;
	float warn {1000}, crit {5000};
	uint32_t port {};

	po::options_description opts("Check ICE object");
	// clang-format off
	opts.add_options()
		("host,H", po::value(&host)->default_value("localhost"), "Host")
		("port,p", po::value(&port)->required(), "Port")
		("protocol,P", po::value(&protocol)->default_value("tcp"), "Protocol")
		("object,o", po::value(&objects)->required(), "Object")
		("warn,w", po::value(&warn)->default_value(1000), "Warning at millisecond")
		("crit,c", po::value(&crit)->default_value(5000), "Critical at millisecond")
		("help,h", "Show help")
		;
	// clang-format on
	try {
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv)
						  .positional(po::positional_options_description().add("host", 1).add("object", -1))
						  .options(opts)
						  .run(),
				vm);

		if (vm.count("help")) {
			std::cerr << opts << std::endl;
			return 3;
		}
		po::notify(vm);

		auto ic = Ice::initialize(argc, argv);
		int rtn = 0;

		std::cout << "ICE";
		for (const auto & object : objects) {
			try {
				auto p = ic->stringToProxy(EndpointFmt::get(object, protocol, host, port));
				auto t_taken_ms = stopwatch([&p] {
					p->ice_ping();
				});
				Good::write(std::cout, object, t_taken_ms);
				if (t_taken_ms > crit) {
					atLeast(rtn, 2);
				}
				if (t_taken_ms > warn) {
					atLeast(rtn, 1);
				}
			}
			catch (const std::exception & ex) {
				Bad::write(std::cout, object, ex.what());
				atLeast(rtn, 2);
			}
		}
		ic->destroy();
		std::cout << "\n";
		return rtn;
	}
	catch (const std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return 3;
	}
}
