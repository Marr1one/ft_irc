/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlackjackGame.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 20:37:25 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/19 00:33:03 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BlackjackGame.hpp"

#include <cstdlib>
#include <ctime>
#include <sstream>

BlackjackGame::BlackjackGame() : _active(false), _finished(false) {}

static void seedRandomOnce() {
	static bool seeded = false;

	if (!seeded) {
		std::srand(static_cast<unsigned int>(std::time(NULL)));
		seeded = true;
	}
}

int BlackjackGame::drawCard() {
	seedRandomOnce();
	return (std::rand() % 13) + 1;
}

int BlackjackGame::handValue(const std::vector<int> &hand) {
	int total = 0;
	int aces = 0;

	for (std::vector<int>::const_iterator it = hand.begin(); it != hand.end();
		 ++it) {
		if (*it == 1) {
			total += 11;
			++aces;
		} else if (*it >= 10)
			total += 10;
		else
			total += *it;
	}
	while (total > 21 && aces > 0) {
		total -= 10;
		--aces;
	}
	return total;
}

std::string BlackjackGame::cardLabel(int card) {
	if (card == 1)
		return "A";
	if (card == 11)
		return "J";
	if (card == 12)
		return "Q";
	if (card == 13)
		return "K";
	std::ostringstream oss;
	oss << card;
	return oss.str();
}

std::string BlackjackGame::handToString(const std::vector<int> &hand) {
	std::string result;

	for (std::vector<int>::const_iterator it = hand.begin(); it != hand.end();
		 ++it) {
		if (!result.empty())
			result += ", ";
		result += cardLabel(*it);
	}
	return result;
}

void BlackjackGame::start() {
	_playerHand.clear();
	_dealerHand.clear();
	_active = true;
	_finished = false;
	_playerHand.push_back(drawCard());
	_playerHand.push_back(drawCard());
	_dealerHand.push_back(drawCard());
	_dealerHand.push_back(drawCard());
	if (playerBlackjack() || dealerBlackjack())
		_finished = true;
}

void BlackjackGame::playerHit() {
	if (!_active || _finished)
		return;
	_playerHand.push_back(drawCard());
	if (playerBust())
		_finished = true;
}

void BlackjackGame::playerStand() {
	if (!_active || _finished)
		return;
	while (getDealerValue() < 17)
		_dealerHand.push_back(drawCard());
	_finished = true;
}

void BlackjackGame::reset() {
	_active = false;
	_finished = false;
	_playerHand.clear();
	_dealerHand.clear();
}

bool BlackjackGame::isActive() const { return _active; }

bool BlackjackGame::isFinished() const { return _finished; }

bool BlackjackGame::playerBust() const { return getPlayerValue() > 21; }

bool BlackjackGame::dealerBust() const { return getDealerValue() > 21; }

bool BlackjackGame::playerBlackjack() const {
	return _playerHand.size() == 2 && getPlayerValue() == 21;
}

bool BlackjackGame::dealerBlackjack() const {
	return _dealerHand.size() == 2 && getDealerValue() == 21;
}

int BlackjackGame::getPlayerValue() const { return handValue(_playerHand); }

int BlackjackGame::getDealerValue() const { return handValue(_dealerHand); }

std::string BlackjackGame::getPlayerHandString() const {
	return handToString(_playerHand);
}

std::string BlackjackGame::getDealerHandString(bool revealAll) const {
	if (revealAll || _dealerHand.empty())
		return handToString(_dealerHand);
	if (_dealerHand.size() == 1)
		return cardLabel(_dealerHand[0]);
	return cardLabel(_dealerHand[0]) + ", ?";
}

std::string BlackjackGame::getResult() const {
	const int player = getPlayerValue();
	const int dealer = getDealerValue();

	if (!_finished)
		return "Game still in progress.";
	if (playerBust())
		return "Bust. Dealer wins.";
	if (dealerBust())
		return "Dealer busts. You win.";
	if (playerBlackjack() && dealerBlackjack())
		return "Push. Both have blackjack.";
	if (playerBlackjack())
		return "Blackjack. You win.";
	if (dealerBlackjack())
		return "Dealer has blackjack. You lose.";
	if (player > dealer)
		return "You win.";
	if (dealer > player)
		return "Dealer wins.";
	return "Push.";
}
