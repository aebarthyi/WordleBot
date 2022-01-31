#include<dpp/dpp.h>
#include<regex>
#include<iomanip>
#include"Database.h"

int main() {
	dpp::cluster bot("OTM2MDIyNjUwMTQ3NDY3MzM2.YfHJBg.ncO3It7XeHdaJGx8UOG5JCd0dKI");
	Database data;

	bot.on_ready([&bot](const dpp::ready_t& event) {
		std::cout << "Logged in as " << bot.me.username << "!\n";
	});

	bot.on_message_create([&bot,&data](const dpp::message_create_t& event) {

		if (event.msg.channel_id != 936722516842537031)
			return;

		std::regex r("[a-zA-Z]{6} [0-9][0-9][0-9] [0-6X]/6");

		if (event.msg.content._Starts_with("Wordle")) {
			if (!std::regex_match(event.msg.content.substr(0, 14), r)) {
				bot.message_create(dpp::message(event.msg.channel_id,
					"Invalid format please try to paste it correctly."
				));
				return;
			}
			
			std::string grade;
			dpp::user player = event.msg.author;
			int wordleNum = std::stoi(event.msg.content.substr(7, 3));
			uint64_t guesses;
			if (event.msg.content.at(11) == 'X')
				guesses = 7;
			else
				guesses = std::stoi(event.msg.content.substr(11, 1));

			switch (guesses) {
			case 1 :
				grade = "HOLY SHIT YOU ARE ACTUALLY CHEATING THERE'S NO WAY!";
				break;
			case 2:
				grade = "DAAAAAAMN THATS PRETTY INSANE!";
				break;
			case 3:
				grade = "Very nice.";
				break;
			case 4:
				grade = "Pretty good.";
				break;
			case 5:
				grade = "Pretty average.";
				break;
			case 6:
				grade = "DOG SHIT, but hey, at least you got it.";
				break;
			default:
				grade = "Better luck next time :)";
				break;
			}

			bot.message_create(dpp::message(event.msg.channel_id,
				grade
			));

			if (!data.playerExists(player.id)) 
				data.addPlayer(player.id, player.username);
			
			data.updateStats(player.id, guesses, wordleNum);
			
			if (!data.wordleExists(wordleNum)) 
				data.addWordle(wordleNum);

			data.updateWordleStats(wordleNum, guesses);

		return;
		}

		else if (event.msg.content._Starts_with("!wordle stats")) {
			dpp::user player = event.msg.author;
			statStruct stats;
			data.retrieveStats(player.id, stats);
			bot.message_create(dpp::message(event.msg.channel_id,
				"Stats for:  " +
				player.username +
				"\nTotal Guesses: " +
				std::to_string(stats.guesses) +
				"\nTotal Possible: " +
				std::to_string(stats.possible) +
				"\nRatio: " +
				std::to_string(stats.ratio)
			));
		return;
		}

		else if (event.msg.content._Starts_with("!wordle leaderboard")) {
			dpp::embed leaderboardEmb;
			dpp::user* player;

			leaderboardEmb.set_title("Wordle Top Guessers:");

			std::vector<statStruct>leaderboard = data.retrieveLeaderboard();

			for (statStruct& s : leaderboard) {
				player = dpp::find_user(s.userID);
				leaderboardEmb.add_field(
					player->username,
					"\nGuesses: " + std::to_string(s.guesses) + 
					"\nPossible: " + std::to_string(s.possible) +
					"\nWordles: " + std::to_string(s.possible / 6) +
					"\nRatio: " + std::to_string(s.ratio)
				);
			}
			bot.message_create(dpp::message(event.msg.channel_id, leaderboardEmb));
		return;
		}

		else if (event.msg.content._Starts_with("!wordle today")) {
			uint64_t stats[3]{ 0 };
			data.retrieveWordleStats(stats);
			dpp::embed wordle;
			wordle.set_title("Wordle: " + std::to_string(stats[0]));
			wordle.add_field("Average guesses: ",
				std::to_string(((float)stats[1]) / ((float)stats[2] / 6.0))
			);
			bot.message_create(dpp::message(event.msg.channel_id, wordle));
		}
	});

	bot.start(false);

	return 0;
}