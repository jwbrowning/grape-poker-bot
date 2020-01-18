# grape-poker-bot
A program that plays heads up no limit hold'em by balancing its ranges at every decision point based on bet sizing.

Grape is designed to be compatible with another one of my projects, The Lion's Den, which is a program that allows two poker bots to play against each other by communicating using text files.

# Data versus other bots

After simulating over 2000 hands against Big Jim, Grape was up over 1000 big blinds, winning at a rate of roughly 51.76 bb/100, which is quite significant.

Because of a known issue, however, Grape performs poorly against another bot, Bluff God (a bot designed to go all in at every chance). After simulating 3000 hands against Bluff God, Grape was losing at a rate of -39.97 bb/100. 

It should be noted that while Grape consistently beats Jim and loses to Bluff God, Big Jim consistently beats Bluff God, forming an interesting triangle of sorts.
