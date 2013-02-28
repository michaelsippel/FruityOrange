/**
 *  kernel/driver/kbc/translate.c
 *
 *  (C) Copyright 2012 Michael Sippel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stdbool.h>

#include <driver/keyboard.h>

int kbd_assignment_id = 0;

uint8_t std_scancode_keycode[] = {0, 
      1,15,16,17,18,19,20,21,22,23,24,25,26,65,27,
     28,29,30,31,32,33,34,35,36,37,38,39,66,68,41,
     42,43,44,45,46,47,48,49,50,51,14,53,52,55,56,
     57,58,59,60,61,62,63,64,67,76,56,71,40, 2, 3,
      4, 5, 6, 7, 8, 9,10,11,00,00,00,76,00,00,77,
     00,79,00,00,78,00,00,00,00,00,00,12,13
};

uint8_t keycode_ascii[][3][76] = {
  {//DE-de
    {0,//Normal
     'E','F','F','F','F','F','F','F','F','F','F','F','F',
     '^','1','2','3','4','5','6','7','8','9','0',225,'´',
    '\t','q','w','e','r','t','z','u','i','o','p','ü','+',
     0,'a','s','d','f','g','h','j','k','l', 148, 132,'#',
     0,'<','y','x','c','v','b','n','m',',','.','-',
     0,'\n',0,0,0,0,' ',0,0,0
    },
    {0,//Shift
      'E','F','F','F','F','F','F','F','F','F','F','F','F',
      '°','!','"','§','$','%','&','/','(',')','=','?','`',
      '\t','Q','W','E','R','T','Z','U','I','O','P',154,'*',
      0,'A','S','D','F','G','H','J','K','L','Ö',142,'\'',
      0,'>','Y','X','C','V','B','N','M',';',':','_',
	0,'\n',0,0,0,0,' ',0,0,0
    },
    {0,//Alt Gr
      'E','F','F','F','F','F','F','F','F','F','F','F','F',
      '°','¹','²','³','¼','½','¬','{','[',']','}','\\','¸',
      '\t','@','w','e','r','t','z','u','i','o','p','ü','~',
      0,'a','s','d','f','g','h','j','k','l','ö','^','’',
      0,'|','»','«','c','v','b','n','m',',','.','-',
      0,'\n',0,0,0,0,' ',0,0,0
    }
  },{//EN-US
    {0,//Normal
      'E','F','F','F','F','F','F','F','F','F','F','F','F',
      '~','1','2','3','4','5','6','7','8','9','0','-','=',
      '\t','q','w','e','r','t','y','u','i','o','p','[',']',
      0,'a','s','d','f','g','h','j','k','l',';','\'','\\',
      0,'\\','z','x','c','v','b','n','m',',','.','/',
      0,'\n',0,0,0,0,' ',0,0,0
    },
    {0,//Shift
      'E','F','F','F','F','F','F','F','F','F','F','F','F',
      '`','!','@','#','$','%','^','&','*','(',')','_','+',
      '\t','Q','W','E','R','T','Y','U','I','O','P','{','}',
      0,'A','S','D','F','G','H','J','K','L',':','\"','|',
      0,'|','Z','X','C','V','B','N','M','<','>','?',
	0,'\n',0,0,0,0,' ',0,0,0
    },
    {0,//Alt Gr
      'E','F','F','F','F','F','F','F','F','F','F','F','F',
      '°','¹','²','³','¼','½','¬','{','[',']','}','\\','¸',
      '\t','q','w','e','r','t','y','u','i','o','p','ü','~',
      0,'a','s','d','f','g','h','j','k','l','ö','^','’',
      0,'|','»','«','c','v','b','n','m',',','.','-',
      0,'\n',0,0,0,0,' ',0,0,0
    }
  }
};

uint8_t translate_scancode(int type, uint16_t scancode){
  uint8_t keycode = 0;
  switch(type){
    // Normal scancode
    case SNC_TYPE_STD:
      keycode = std_scancode_keycode[scancode];
      break;
    // Extented0 scancodes
    case SNC_TYPE_E0:
      break;
    // Extented1 scancodes
    case SNC_TYPE_E1:
      break;
  }
  return keycode;
}

uint8_t translate_keycode(uint8_t keycode, int modus){
  uint8_t ascii = 0;
  /**
   * @p modus normal,shift,altgr
   * 	0 - Normal
   * 	1 - Shift
   * 	2 - Alt Gr
   */
  ascii = keycode_ascii[kbd_assignment_id][modus][keycode];
  return ascii;
}

void set_kbd_assignment(int id) {
  kbd_assignment_id = id;
}
