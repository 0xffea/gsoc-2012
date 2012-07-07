#!/usr/bin/dtrace -qs

#pragma D option flowindent


syscall::connect:entry
/execname == "client"/
{
	self->traceme = 1;
}

fbt:::
/self->traceme/
{}

syscall::connect:return
/self->traceme/
{
	self->traceme = 0;
	exit(0);
}

