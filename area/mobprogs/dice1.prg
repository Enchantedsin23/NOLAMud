>speech_prog bet 50~
if goldamt($n) < 50
  mpecho Harrison the diceroller exclaims "You don't have enough money to bet 50 gold pieces!"
  break
else
  mpecho Harrison smiles and says 'Well of course I will take your bet sir'
  if rand(20)
    mpecho Harrison exclaims 'We have a winner!'
    mpgold 300
    mpforce $n 'give 50 coins $i' 
    give 300 coins $n
  else
    mpecho Harrison frowns and says 'I'm sorry you didn't win.  Please try again.'
  endif
endif
~
>bribe_prog 100~
if rand(20)
  say We have a winner!
  mpgold 500
  give 500 coins $n
else
  say I'm sorry you didn't win.  Please try again.
endif
~
|
