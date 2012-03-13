#!/bin/sh
#############################################################################
# Copyright(c) 2007-2012 Quake2World.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or(at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#############################################################################


CURRENTARCH=`gcc -v 2>&1|grep Target|cut -d\  -f2|cut -d\- -f1`

if [ -z $CURRENTARCH ]; then
  echo "/mingw is not mounted or gcc not installed"
fi


function BUILD
{
	rm -f _build-*.log
	sh ../switch_arch.sh
	
	CURRENTARCH=`gcc -v 2>&1|grep Target|cut -d\  -f2|cut -d\- -f1`

	gcc -v >> _build-"$CURRENTARCH".log 2>&1
	sh _build_win32.sh >> _build-"$CURRENTARCH".log 2>&1
	
	if [ $? != "0" ];then
		echo "Build error"
		sync
    	./_rsync_retry.sh -vrzhP --timeout=120 --chmod="u=rwx,go=rx" -p --delete --inplace --rsh='ssh'  _build-"$CURRENTARCH".log web@satgnu.net:www/satgnu.net/files
		mailsend.exe -d jdolan.dyndns.org -smtp jdolan.dyndns.org -t quake2world-dev@jdolan.dyndns.org -f q2wbuild@jdolan.dyndns.org -sub "Build FAILED svn-$NEWREV on $CURRENTARCH" +cc +bc -a "_build-$CURRENTARCH.log,text/plain"
	else
		echo "Build succeeded"
		sync
    	./_rsync_retry.sh -vrzhP --timeout=120 --chmod="u=rwx,go=rx" -p --delete --inplace --rsh='ssh'  _build-"$CURRENTARCH".log web@satgnu.net:www/satgnu.net/files
		rm _build-"$CURRENTARCH".log
	fi
}

while true; do
	CURREV=`svn info			$CURRENTARCH/quake2world|grep Revision:|cut -d\  -f2`
	NEWREV=`svn info svn://jdolan.dyndns.org/quake2world|grep Revision:|cut -d\  -f2`

	echo $CURREV $NEWREV

	if [ $CURREV != $NEWREV -o -e *.log ];then
		echo "Building" - `date`
		BUILD
		BUILD
		echo "Building done" - `date`
	else
		echo "Nothing has changed and no previsouly failed build." - `date`
	fi

	sleep 3h
	
done
