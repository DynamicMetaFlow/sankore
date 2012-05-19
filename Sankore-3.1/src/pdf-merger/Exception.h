/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef EXCEPTION_H
#define EXCEPTION_H


#include <exception>
#include <string>
#include <sstream>
#include <iostream>


namespace merge_lib
{
   class Exception : public std::exception
   {
   public:
      Exception() {}

      Exception(const char * message) : _message(message) {}
      
      Exception(std::string & message) : _message(message) {}

      Exception(std::stringstream & message) : _message(message.str()) {}

      Exception(const std::string & message) : _message(message) {}
      
      virtual ~Exception() throw () {}
      
      virtual const char * what() const throw() { return _message.c_str(); }

      void show() const {}

   protected:
      std::string _message;
   };
}
#endif // EXCEPTION_HH
