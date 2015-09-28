# test large data files
../../bigt -n 60000
../../bigtdel -n 60000
../../bt <<EOF
open test_db
find
list-keys
quit
EOF
