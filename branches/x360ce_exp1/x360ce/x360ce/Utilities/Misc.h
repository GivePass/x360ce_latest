/*  x360ce - XBOX360 Controler Emulator
 *  Copyright (C) 2002-2010 Racer_S
 *  Copyright (C) 2010-2011 Robert Krawczyk
 *
 *  x360ce is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  x360ce is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with x360ce.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

LPCWSTR HostFileName();
LPCWSTR DLLFileName(HINSTANCE hModule);
LPWSTR const DXErrStr(HRESULT dierr);
inline LONG clamp(LONG val, LONG min, LONG max)
{
	if (val < min) return min;

	if (val > max) return max;

	return val;
}
inline LONG deadzone(LONG val, LONG min, LONG max, LONG lowerDZ, LONG upperDZ)
{
	if (val < lowerDZ) return min;

	if (val > upperDZ) return max;

	return val;
}
#if 0
HRESULT GUIDtoString(const GUID pg, LPWSTR str, int size);
#endif
LPCWSTR GUIDtoString(const GUID g, LPWSTR str);
LPCSTR GUIDtoString(const GUID g, LPSTR str);

void StringToGUID(LPCWSTR szBuf, GUID *rGuid);

#endif // _UTILS_H_