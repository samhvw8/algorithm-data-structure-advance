#!/bin/sh
../../bt <<EOF
create test_db
execute ../defdata.bt
define-root test
execute ../defdata.bt
quit
EOF
../../kcp test_db new_db
echo "Files should be identical"
../../bt <<EOF
open new_db
find
list-keys
open test_db
find
list-keys
quit
EOF
echo "Changing test_db..."
../../bt <<EOF
open test_db
execute ../remdata.bt
quit
EOF
../../kcp test_db new_db
echo "Files should now differ"
../../bt <<EOF
open new_db
find
list-keys
open test_db
find
list-keys
quit
EOF
rm new_db
