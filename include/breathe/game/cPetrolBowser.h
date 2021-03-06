#ifndef CPETROLBOWSER_H
#define CPETROLBOWSER_H

namespace breathe
{
  const unsigned int PETROL_STOCK=0;
  const unsigned int PETROL_PREMIUM=1;
  const unsigned int PETROL_RACING=2;
  const unsigned int PETROL_SIZE=3;

  class cPetrolBowser
  {
  public:
    unsigned int uiType;

    float fHealth;

    float fPrice;

    cPetrolBowser(unsigned int type, float price);
    ~cPetrolBowser();
  };
}

#endif // CPETROLBOWSER_H
