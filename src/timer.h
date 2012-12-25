/* timer

   Copyright 2012  Oleg Kutkov <elenbert@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */

#ifndef NTSTAT2_TIMER_H
#define NTSTAT2_TIMER_H

typedef void (*timer_callback) (void* cookie);

void set_timer_callback(timer_callback func);

int start_timer(unsigned int timeout_ms, void* thread_arg);
void stop_timer();

#endif //NTSTAT2_TIMER_H

