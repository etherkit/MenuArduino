/*
 * Menu.h - Generic menu class library for Arduino
 *
 * Copyright (C) 2018 Jason Milldrum <milldrum@gmail.com>
 *
 * Makes use of modified Vector.h from Tom Stewart, since Arduino doesn't play
 * nicely with the STL yet.
 * https://github.com/tomstewart89/Vector
 *
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

#ifndef ETHERKIT_MENU_H_
#define ETHERKIT_MENU_H_

#include "Vector.h"

constexpr uint8_t label_size = 20;

enum class MenuType {Directory, Action, Setting, Invalid};

class BaseMenu
{
public:
  BaseMenu(BaseMenu*, const char*, MenuType);
  virtual MenuType select() {};

  BaseMenu* parent;
  static BaseMenu* active_menu;
  static uint8_t active_item;
  char label[label_size];
  bool selected = false;
  Vector<BaseMenu*> submenus;
  MenuType type;
};

class DirectoryMenu : public BaseMenu
{
public:
  DirectoryMenu(BaseMenu*, const char*);
  ~DirectoryMenu();
  MenuType select();
  void addDirectoryMenu(const char*);
  void addActionMenu(const char*, void (*_callback)(void));
  void addActionMenu(const char*, void (*_callback)(uint8_t), const uint8_t);
  void addSettingMenu(const char*, void (*_callback)(const char *), const char *);
};

class ActionMenu : public BaseMenu
{
public:
  ActionMenu(BaseMenu*, const char*, void (*_callback)(void));
  ActionMenu(BaseMenu*, const char*, void (*_callback)(uint8_t), const uint8_t);
  MenuType select();

  void (*callback)(void) = nullptr;
  void (*callback_uint)(uint8_t) = nullptr;
  uint8_t val = 0;
};

class SettingMenu : public BaseMenu
{
public:
  SettingMenu(BaseMenu*, const char *, void (*_callback_key)(const char *), const char *);
  MenuType select();

  void (*callback_key)(const char *) = nullptr;
  const char * key = nullptr;
};

class Menu
{
public:
  Menu();
  Menu(const char*);
  ~Menu();
  MenuType selectChild(const uint8_t);
  //bool selectChild(const uint8_t, const uint8_t);
  MenuType selectActiveChild();
  MenuType selectActiveChild(const uint8_t);
  bool selectParent();
  void selectRoot();
  void addChild(const char*);
  void addChild(const char*, void (*callback)(void));
  void addChild(const char*, void (*callback)(uint8_t), const uint8_t);
  void addChild(const char*, void (*callback)(const char *), const char *);
  uint8_t countChildren();
  Vector<BaseMenu*> getChildren();
  BaseMenu* getActiveChild();
  BaseMenu* getActiveChild(const uint8_t);
  const char* getActiveChildLabel();
  const char* getActiveChildLabel(const uint8_t);
  bool setActiveChild(uint8_t);
  bool atRoot();
  void operator++();
  void operator++(int);
  void operator--();
  void operator--(int);

  DirectoryMenu* root_menu = nullptr;
  uint8_t active_child = 0;
  //uint16_t active_item;
};

#endif // ETHERKIT_MENU_H_
