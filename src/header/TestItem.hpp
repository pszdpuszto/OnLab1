#pragma once

#include "Item.hpp"

class RubyHealingRing : public Item {
public: RubyHealingRing() : Item{ "Ruby healing ring", USEFUL, RING,
	{} } {}
};

class VampireAmulet : public Item {
public: VampireAmulet() : Item{ "Vampire amulet", RARE, AMULET,
	{
		{ Stats::LIFESTEAL, .4f }
	}
} {}
};

class LeatherJacket : public Item {
public: LeatherJacket() : Item{ "Leather Jacket", BORING, ARMOR,
	{
		{ Stats::ARMOR, 0.2f }
	}
} {
}
};

class CrusaderBoots : public Item {
public: CrusaderBoots() : Item{ "Crusader boots", RARE, BOOTS,
	{
		{ Stats::SPD, 0.2f },
		{ Stats::ARMOR, 0.2f }
	}
} {
}
};

class BaseballCap : public Item {
public: BaseballCap() : Item{ "Baseball Cap", TRASH, HELMET,
	{
		{ Stats::SPD, 0.1f },
		{ Stats::ATK_SPD, 0.1f },
	}
} {
}
};

class Broom : public Item {
public: Broom() : Item{ "Broom", TRASH, WEAPON,
	{

	}
} {
}
};

class Uzi : public Item {
public: Uzi() : Item{ "Uzi", BORING, WEAPON,
	{
	}
} {
}
};

//class cn : public Item {
//public: cn() : Item{ "name", rarity, type,
//	{
//		{ Stats::SPD,  },
//		{ Stats::HP,  },
//		{ Stats::REGEN,  },
//		{ Stats::ARMOR,  },
//		{ Stats::DMG,  },
//		{ Stats::ATK_SPD,  },
//		{ Stats::LIFESTEAL,  }
//	}
//} {}
//};