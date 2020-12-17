#!/bin/sh
(echo '#!exec/cnex/cnex'; cat $1) > $2
chmod +x $2
