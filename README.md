# check_ice
A trivial [Nagios](https://www.nagios.org/) ([Monitoring Plugins](https://www.monitoring-plugins.org/)
compatible) checker for testing the availability of [ZeroC](https://zeroc.com/)
[ICE](https://zeroc.com/products/ice) service objects.

## Usage

* `-H` | `--host` arg: Host name of service (default `localhost`).
* `-p` | `--port` arg: Port number of service.
* `-P` | `--protocol` arg: Protocol name (default `tcp`).
* `-o` | `--object` arg: Object name (can be specified multiple times).
* `-w` | `--warn` arg: Response time which will trigger a warning result (default `1000`ms).
* `-s` | `--crit` arg: Response time which will trigger a critical result (default `5000`ms).
* `-h` | `--help`: Show help

Example:

    ./check_ice --host my.service.lan --port 4000 --object auth --object api

Or in the short form:

    ./check_ice <host> <other options> <object 1> [<object 2> ... <object N>]

Example:

    ./check_ice my.service.lan -p 4000 auth api

### Example Nagios command
    define command {
        command_name    check_ice
        command_line    $USER1$/check_ice $HOSTADDRESS$ -p $ARG1$ $ARG2$ $ARG3$ $ARG4$ $ARG5$
    }

### Example Nagios service
    define service {
        use                     local-service
        hostgroup_name          api-servers
        service_description     MyAPI
        check_command           check_ice!4000!auth!api
    }

## Output

    ICE OK: auth 3.4723ms OK: api 0.928027ms

## Return value
In keeping with the existing Monitoring Plugins specification:

* `0` OK; *all* objects responded in a timely manner.
* `1` warning; at least one object took longer than warning time to respond.
* `2` critical; at least one object took longer than critical time to respond or was unreachable.
* `3` no checks performed; help, bad options, etc.

## Method of operation

`check_ice` creates a proxy for each object provided on the command line in turn and calls `ice_ping()` on it.
The ping operation is timed for each object separately. It is likely that after the first object responds, connection reuse will result in much lower ping times for subsequent objects.

## Installation
### Requirements
* [Compile Time Formatter](https://github.com/rascalDan/libadhocutil/blob/master/libadhocutil/compileTimeFormatter.h)
* [Boost Build](https://boostorg.github.io/build/manual/develop/index.html)
* [Boost Program Options](https://www.boost.org/doc/libs/1_69_0/doc/html/program_options.html)

### Compilation
    bjam
    bjam install
