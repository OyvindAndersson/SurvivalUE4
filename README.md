# Survival UE4 Project

##! NOTE !: 
**This is an old learning project that never finished properly, kept for nostalgic purposes. 
I cannot guarantee it all works now as described, but most of the inventory stuff
should be working afaik.**

Singleplayer puzzle/adventure/horror game relying heavily upon item interaction and problemsolving.

## Inventory System
The inventorysystem can best be described as a "clone" of Resident Evil 2, for instance.
Item types include, but not limited to:

+ Item: Any usable item (base class)
+ Weapon: Any weapon, stored in items inventory. Decorators decide speciality (Projectile, Bludgeon, etc)
+ Key: Stored in items inventory. Cannot be discarded, as they are key to progression
+ File: File items are stored in their own inventory; can never be discarded.

## Crafting
The crafting system is a very basic - two items can be combined to form another.
Combinations are not known and must be discovered by the player.

*Example: ItemA + ItemB yields ItemX*

Two items of the same type can be combined to either yield another item, or
fill up empty room in the max stack size. 

I.e:
+ "Mini-Health" + "Mini-Health" = "Medium-Health"
+ "9mm Ammo" + "9mm Ammo" = First "9mm Ammo" stack size takes what it can from the second "9mm Ammo"'s stack size.

Valid combinations are defined by ``` class UItemCraftRecipe ``` which has its own object factory in the editor (Create new asset -> Misc -> Item Craft Recipe)
These are loaded on game init and searched whenever a combination of items in the inventory UI are signaled.
If the items can be combined, they will be removed and replaced with the "yield" item, or in case of stackable combination,
the stacksizes of the respective items will be updated.

## Status Ailment System
The player can be inflicted with status ailments that are either positive or negative.
Let's take two black/white examples:
+ Poison: Gradually decreases the player health, and add onscreen effects / other stat changes.
+ Adrenaline: Increases player speed, damage is less effective on player etc.

### Base classes

## Weapon system

Weapons need a specific ammo type to be loaded. Thus the player must find ammunition for the respective weapon
to load and fire. Each firearm has a max clip-size that will take from available ammo items, decreasing their stack size.
Weapons that do not need ammunition have a clip-size of 0 and can be "fired" infinetly.

Standard weaponry:
+ Pistols
+ Shotguns
+ Rifles
+ Grenades
+ Bludgeons (Axe, knife)

Handheld utilities (Equipped same as weapons)
+ Flashlight (Seconhand equip : Can be used alongside with weapons)

### Weapon item states

+ UWeaponState
+ UWeaponState_Firing
+ UWeaponState_Reloading
+ UWeaponState_Equipping
+ UWeaponState_UnEquipping
+ UWeaponState_Idle

#### Statemachine

UBaseWeaponItem
+ TArray<TSubclassOf<UWeaponState>> States
+ ChangeState(TSubclassOf<UWeaponState> NewState, TSubclassOf<UWeaponState> PrevState)
	
	
	
	
	
	
	
	
