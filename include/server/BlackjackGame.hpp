/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlackjackGame.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 20:36:02 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/19 00:30:04 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLACKJACKGAME_HPP
#define BLACKJACKGAME_HPP

#include <string>
#include <vector>

class BlackjackGame {
  private:
	bool _active;
	bool _finished;
	std::vector<int> _playerHand;
	std::vector<int> _dealerHand;

	static int drawCard();
	static int handValue(const std::vector<int> &hand);
	static std::string cardLabel(int card);
	static std::string handToString(const std::vector<int> &hand);

  public:
	BlackjackGame();

	void start();
	void playerHit();
	void playerStand();
	void reset();

	bool isActive() const;
	bool isFinished() const;
	bool playerBust() const;
	bool dealerBust() const;
	bool playerBlackjack() const;
	bool dealerBlackjack() const;

	int getPlayerValue() const;
	int getDealerValue() const;
	std::string getPlayerHandString() const;
	std::string getDealerHandString(bool revealAll) const;
	std::string getResult() const;
};

#endif
