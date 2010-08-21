/*
** Copyright (c) 2010 Michael Dvorkin
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the Simplified BSD License (also
** known as the "2-Clause License" or "FreeBSD License".)
**
** This program is distributed in the hope that it will be useful,
** but without any warranty; without even the implied warranty of
** merchantability or fitness for a particular purpose.
*/
#if !defined(__FORMAT_H__)
#define __FORMAT_H__

#define FORMAT_ACTION  1
#define FORMAT_PROJECT 2
#define FORMAT_TASK    4
#define FORMAT_NOTE    8

typedef struct _Format {
    int   type;
    int   indent;
    int   number_of_entries;
    char *entries;
    union {
        struct {
            int id;
            int username;
            int name;
            int status;
        } project;
        struct {
            int id;
            int username;
            int name;
            int status;
            int priority;
            int date;
            int time;
        } task;
        struct {
            int username;
            int message;
        } action;
        struct {
            int id;
            int username;
        } note;
    } max;
} Format, *PFormat;

PFormat pit_format_initialize(int type, int indent, int number_of_entries);
void    pit_format(PFormat pf, char *entry);
void    pit_format_flush(PFormat pf);
void    pit_format_free(PFormat pf);

#endif