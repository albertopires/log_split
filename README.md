# Log_split #

Log_split reads data from stdin and writes to a log file, when this log file reaches
a specified size it is split.  
The idea is to have a clean way to handle output from a program that is usually send
to a log file using something like "exemple_program > output.log". While this works fine,
sometimes the log file may grow very fast or to a very large size and than the only
way to erase o clean the log file is to restart to program.  
So far I couldn't find a way to clean the log file so I wrote log_split, that way
I can have a clean way to compress or delete chunks of log data and also the option
to turn on/off log writing via SIGHUP, witch can be very useful during debugging.

### Example ###
```
alberto@home:~/git/log_split (master)$ ./log_split
./log_split <log_name> <max_log_size in MB>
alberto@home:~/git/log_split (master)$ ./any_program | ./log_split /var/log/any_program.log 100"

```

in this example every time any_program.log reaches 100 MB it will be renamed and a new one
will be created.  
To turn on/off the output writing just send a signal hangup to the log_split process.

```
alberto@home:~/git/log_split (master)$ kill -HUP `pidof log_split`
```
