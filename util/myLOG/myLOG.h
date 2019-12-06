/*
    Defines API for my Logging utility
    Copyright (C) 2019  Michael Heil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Created: 08.11.2019 10:47:24
 *  Author: Michael
 */ 


#ifndef MYLOG_H_
#define MYLOG_H_

void LOG_error(char * message);
void LOG_info(char * message);
void LOG_debug(char * message);

#endif /* MYLOG_H_ */