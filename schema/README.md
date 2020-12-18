# timing_job.json generation instructions
To generate the json file to be fed into the restcmd interface, run the command below. The path given under option '-M' should point to your location of the appfwk/schema directory.

moo -M ../../../../appfwk/schema compile timing_job.jsonnet
