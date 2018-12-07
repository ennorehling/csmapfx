# !/bin/sh

REVISION=$(svn info | grep 'Revision' | grep -o '[0-9]\+')
DATELINE=$(svn info | grep 'Änderungsdatum')
DATE=$(echo $DATELINE | grep -o '[0-9]\+-[0-9-]\+')
TIME=$(echo $DATELINE | grep -o '[0-9]\+:[0-9:]\+')

if [ -z $1 ]; then
   echo "Usage: $0 <inputfile>"
   echo "   or: $0 -r"
   exit
fi

if [ $1 = -r ]; then
   echo $REVISION
   exit
fi

sed -e 's/\$WCREV\$/'$REVISION'/g' -e 's/\$WCDATE\$/'$DATE' '$TIME'/g' $1
