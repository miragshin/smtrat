#!/usr/bin/env bash

mkdir build || return 1
cd build/ || return 1
cmake -D DEVELOPER=ON -D SMTRAT_Strategy=AllModulesStrategy ../ || return 1

if [[ ${TASK} == "doxygen" ]]; then
	make doc
	
	git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
	git config --global user.name "Travis doxygen daemon"
	
	git clone https://${GH_TOKEN}@github.com/smtrat/smtrat.github.io.git
	cd smtrat.github.io/ || return 1
	
	# Update cloned copy
	cp -r ../doc/html/* ./ || return 1
	git add . || return 1
	# Commit and push
	git commit -m "Updated documentation for SMT-RAT" || return 1
	git push origin master || return 1

else
	/usr/bin/time make -j1 || return 1
fi

cd ../
