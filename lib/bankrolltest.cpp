#include <iostream>
#include "Bankroll.hpp"

using namespace std;

int main() {
	Bankroll pBankroll = Bankroll::instance();
		cout << pBankroll.get_capital() << endl;
		pBankroll.set_initial_capital(1000);
		

	cout << pBankroll.get_capital() << endl;
	
	pBankroll.update_capital(444000);
	cout << pBankroll.get_capital() << endl;
	
	return 0;
}

