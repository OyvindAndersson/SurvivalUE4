# Survival UE4 Project

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
+ "9mm Ammo" + "9mm Ammo" = First "9mm Ammo" is filled up with whatever it can take from the second Ammo. If its full, nothing happens.

## Weapon system
