#!/usr/bin/dtrace -qs

#pragma D option flowindent


syscall::accept:entry
/execname == "server"/
{
	self->traceme = 1;
}

fbt:::
/self->traceme/
{}

syscall::accept:return
/self->traceme/
{
	self->traceme = 0;
	exit(0);
}

