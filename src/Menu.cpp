/*
 * Menu.cpp - Generic menu class library for Arduino
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

#include <cstring>
#include <memory>

#include "Menu.h"
#include "Vector.h"

BaseMenu* BaseMenu::active_menu = nullptr;
uint8_t BaseMenu::active_item = 0;

/*
 * Morse::Morse(uint8_t tx_pin, float init_wpm) : output_pin(tx_pin)
 *
 * Create an instance of the Morse class.
 *
 * tx_pin - Arduino pin used as the output by this library. Will not toggle an
 *          output pin if set to 0.
 * init_wpm - Sending speed in words per minute.
 *
 */
BaseMenu::BaseMenu(BaseMenu* _parent, const char* _label, MenuType _type)
	: parent(_parent), type(_type)
{
	if(parent != nullptr)
  {
    //parent->submenus.push_back(this); // the STL way
		parent->submenus.PushBack(this);
  }
	strcpy(label, _label);
}

DirectoryMenu::DirectoryMenu(BaseMenu* _parent, const char* _label)
	: BaseMenu(_parent, _label, MenuType::Directory) {}

DirectoryMenu::~DirectoryMenu()
{
	// delete submenu pointers
  for(auto & menu : submenus)
  {
    delete menu;
  }
}

MenuType DirectoryMenu::select()
{
	// if(submenus.Size() > 0)
	// {
  	BaseMenu::active_menu = this;
		return MenuType::Directory;
	// }
	// else
	// {
  // 	return MenuType::Invalid;
	// }
}

void DirectoryMenu::addDirectoryMenu(const char* _label)
{
	new DirectoryMenu(this, _label);
}

void DirectoryMenu::addActionMenu(const char* _label, void (*_callback)(void))
{
	new ActionMenu(this, _label, _callback);
}

void DirectoryMenu::addActionMenu(const char* _label, void (*_callback)(uint8_t), const uint8_t val)
{
	new ActionMenu(this, _label, _callback, val);
}

void DirectoryMenu::addSettingMenu(const char* _label, void (*_callback)(const char *), const char * _key)
{
	new SettingMenu(this, _label, _callback, _key);
}

ActionMenu::ActionMenu(BaseMenu* _parent, const char* _label, void (*_callback)(void))
    : BaseMenu(_parent, _label, MenuType::Action), callback(_callback) {}

ActionMenu::ActionMenu(BaseMenu* _parent, const char* _label, void (*_callback)(uint8_t), const uint8_t _val)
    : BaseMenu(_parent, _label, MenuType::Action), callback_uint(_callback), val(_val) {}

MenuType ActionMenu::select()
{
	if(callback != nullptr)
	{
  	callback();
		return MenuType::Action;
	}
	else if(callback_uint != nullptr)
	{
		callback_uint(val);
		return MenuType::Action;
	}
	else
	{
  	return MenuType::Directory;
	}
}

SettingMenu::SettingMenu(BaseMenu* _parent, const char* _label, void (*_callback)(const char *), const char * _key)
    : BaseMenu(_parent, _label, MenuType::Setting), callback_key(_callback), key(_key){}

MenuType SettingMenu::select()
{
	if(callback_key != nullptr)
	{
		callback_key(key);
		return MenuType::Setting;
	}
	else
	{
  	return MenuType::Directory;
	}
}

Menu::Menu()
{
  root_menu = new DirectoryMenu(nullptr, "Main Menu");
  BaseMenu::active_menu = root_menu;
  BaseMenu::active_item = 0;
	active_child = BaseMenu::active_item;
}

Menu::Menu(const char* label)
{
  root_menu = new DirectoryMenu(nullptr, label);
  BaseMenu::active_menu = root_menu;
  BaseMenu::active_item = 0;
	active_child = BaseMenu::active_item;
}

Menu::~Menu()
{
  delete root_menu;
}

MenuType Menu::selectChild(const uint8_t index)
{
  // Bounds check
	//if(index >= BaseMenu::active_menu->submenus.size()) // the STL way
  if(index >= BaseMenu::active_menu->submenus.Size())
  {
    return MenuType::Invalid;
  }
  else
  {
		// Returns true if the selection was an action
		MenuType action = BaseMenu::active_menu->submenus[index]->select();
		BaseMenu::active_item = 0;
		active_child = BaseMenu::active_item;
		return action;
  }
}

MenuType Menu::selectActiveChild()
{
  return selectChild(BaseMenu::active_item);
}

MenuType Menu::selectActiveChild(const uint8_t offset)
{
  return selectChild(BaseMenu::active_item + offset);
}

bool Menu::selectParent()
{
  if(BaseMenu::active_menu->parent != nullptr)
  {
    BaseMenu::active_menu = BaseMenu::active_menu->parent;
		BaseMenu::active_item = 0;
		active_child = BaseMenu::active_item;
		return false;
  }
  else
  {
    BaseMenu::active_menu = root_menu;
		BaseMenu::active_item = 0;
		active_child = BaseMenu::active_item;
		return true;
  }
	// BaseMenu::active_item = 0;
	// active_child = BaseMenu::active_item;
  //
	// if(BaseMenu::active_menu == root_menu)
	// {
	// 	return true;
	// }
	// else
	// {
	// 	return false;
	// }
}

void Menu::selectRoot()
{
	BaseMenu::active_menu = root_menu;
	BaseMenu::active_item = 0;
	active_child = BaseMenu::active_item;
}

void Menu::addChild(const char* label)
{
  static_cast<DirectoryMenu*>(BaseMenu::active_menu)->addDirectoryMenu(label);
}

void Menu::addChild(const char* label, void (*callback)(void))
{
  static_cast<DirectoryMenu*>(BaseMenu::active_menu)->addActionMenu(label, callback);
}

void Menu::addChild(const char* label, void (*callback)(uint8_t), const uint8_t val)
{
  static_cast<DirectoryMenu*>(BaseMenu::active_menu)->addActionMenu(label, callback, val);
}

void Menu::addChild(const char* label, void (*callback)(const char *), const char * key)
{
  static_cast<DirectoryMenu*>(BaseMenu::active_menu)->addSettingMenu(label, callback, key);
}

uint8_t Menu::countChildren()
{
	//return BaseMenu::active_menu->submenus.size(); // the STL way
  return BaseMenu::active_menu->submenus.Size();
}

Vector<BaseMenu*> Menu::getChildren()
{
  return BaseMenu::active_menu->submenus;
}

BaseMenu* Menu::getActiveChild()
{
	return BaseMenu::active_menu->submenus[BaseMenu::active_item];
}

BaseMenu* Menu::getActiveChild(const uint8_t offset)
{
	if(active_child + offset >= BaseMenu::active_menu->submenus.Size())
	{
		return nullptr;
	}
	else
	{
		return BaseMenu::active_menu->submenus[BaseMenu::active_item + offset];
	}
}

const char* Menu::getActiveChildLabel()
{
	if(active_child >= BaseMenu::active_menu->submenus.Size())
	{
		return "";
	}
	else
	{
		return BaseMenu::active_menu->submenus[BaseMenu::active_item]->label;
	}
	//return BaseMenu::active_menu->submenus[BaseMenu::active_item]->label;
}

const char* Menu::getActiveChildLabel(const uint8_t offset)
{
	if(active_child + offset >= BaseMenu::active_menu->submenus.Size())
	{
		return "";
	}
	else
	{
		return BaseMenu::active_menu->submenus[BaseMenu::active_item + offset]->label;
	}
}

bool Menu::setActiveChild(uint8_t child)
{
	if(child <= countChildren() - 1)
	{
		BaseMenu::active_item = child;
		active_child = BaseMenu::active_item;
		return true;
	}
	else
	{
		return false;
	}
}

bool Menu::atRoot()
{
	if(BaseMenu::active_menu == root_menu)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Menu::operator++()
{
  if(BaseMenu::active_item >= countChildren() - 1)
  {
    BaseMenu::active_item = 0;
		active_child = BaseMenu::active_item;
  }
  else
  {
    ++BaseMenu::active_item;
		active_child = BaseMenu::active_item;
  }
}

void Menu::operator++(int)
{
  if(BaseMenu::active_item >= countChildren() - 1)
  {
    BaseMenu::active_item = 0;
		active_child = BaseMenu::active_item;
  }
  else
  {
    ++BaseMenu::active_item;
		active_child = BaseMenu::active_item;
  }
}

void Menu::operator--()
{
  if(BaseMenu::active_item == 0)
  {
    BaseMenu::active_item = countChildren() - 1;
		active_child = BaseMenu::active_item;
  }
  else
  {
    --BaseMenu::active_item;
		active_child = BaseMenu::active_item;
  }
}

void Menu::operator--(int)
{
  if(BaseMenu::active_item == 0)
  {
    BaseMenu::active_item = countChildren() - 1;
		active_child = BaseMenu::active_item;
  }
  else
  {
    --BaseMenu::active_item;
		active_child = BaseMenu::active_item;
  }
}
