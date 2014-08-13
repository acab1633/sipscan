sipscan
=======

Fast sip server discovery using paralell options requests

Pull requests *very* welcome!

To Build
--------

`make all`

Useage
------

`optscan [options] your_ip start_ip end_ip`

* `[-v[v]]` - verbose output
* `[--output-port]` - causes port to be outputted alongside ip addresses
* `[-l logfile]` - log output to file
* `[-p remote_port]` - remote port to send requests to
* `[-s speed(number]` - scanning speed, 0=slow, 1=normal, 2=fast, 3=very fast. Default is 1. Faster scans mean greater chance of false negatives.
  
Outputs the ip's of any servers that respond to stdout & the logfile.

Also outputs the server name given on -v, and additionally the response code on -vv.
