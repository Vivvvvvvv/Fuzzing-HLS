#!/bin/bash
# My first script
echo "Welcome to HLS Fuzz testing - LegUp!"


################################################################################
# Iteratively Checking                                                         #
################################################################################
reduction_process() {
	cp test.c test.txt # reset everytime, test_ini.c stays unchanged, test.c will updated with newest reduced program 
	echo "Removing line $j in total of $ITERS possible lines."
	# comment out one line based on which_reduction
	if [ $WHICH_REDUCTION -eq 1 ]; then
		# hash variable reduction
		gcc modifyMain.c -o modifyMain &&./modifyMain 2 $FUNC_NAME $j > test_reduced.c
	else 
		# function reduction 
		gcc modifyMain.c -o modifyMain &&./modifyMain 1 $FUNC_NAME $j > test_reduced.c
	fi
	# redo the process (test_ini.c must stay unchanged for later recovering the original file)
	cp test_reduced.c test.txt
	# process for vivado
	gcc modifyMain.c -o modifyMain &&./modifyMain 0 $FUNC_NAME $j > test_Mod.c
	cat test_Mod.c > test_Mod.txt
	# get the new golden result for reduced program
	gcc -m32 test_Mod.c -o test_Mod 
	timeout 5m ./test_Mod > goldenResult.txt
	# feed to LegUp
	timeout 120m make default v > logfile.txt
	VAR_RE=$?
	cp transcript transcript.txt
	gcc extractTrans.c -o extractTrans &&./extractTrans > verilogResult.txt
	echo "Test $i result: " >> reduceResult.txt
	gcc resultCheck.c -o resultCheck &&./resultCheck > reduceResult.txt
	RESULT=$(tail -n2 reduceResult.txt | head -1)
	if [[ "$RESULT" == *"Output does not"* ]]; then
		echo "Not the error line"
	else 
		echo "Problem found on $j"
		echo "$j" >> problemVariable.txt
	fi
	echo "LegUp $j finished.."
}

################################################################################
# Problem Program Reduction                                                    #
################################################################################
reduction() {
	echo "Entering program reduction..."
	VAR_NUM=0
	WHICH_REDUCTION=0
	# bug found! do the reducation
	cp test.c test.txt
	cp test.c test_ini.c

	# always start with func_1 and only modify, the number doesn't matter
	gcc modifyMain.c -o modifyMain &&./modifyMain 0 func_1 1 > test_Mod.c
	cp test_Mod.c test_modify_main.txt
	echo $FUNC_NUM
	LEFT_FUNC=$FUNC_NUM
	cp functionName.txt funcNameTmp.txt
	# checked changed to 1 if function is looped through
	# removed changed to 1 if function is being reduced and removed 
	CHECKED=0
	REMOVED=0
	
	while [ $LEFT_FUNC -gt 0 ]
	do
		FUNC_NAME=$(head -1 funcNameTmp.txt)
		echo "currently reducing $FUNC_NAME"
		# take out the first function
		touch tmp.txt
		tail -n +2 funcNameTmp.txt > tmp.txt
		cp tmp.txt funcNameTmp.txt
		rm -f tmp.txt
		# do the calculation on how many lines/variables can be reduced
		gcc modifyMain.c -o modifyMain &&./modifyMain 0 $FUNC_NAME 1 > test_Mod.c
		cat test_Mod.c > test_Mod.txt
		VAR_NUM=0
		WHICH_REDUCTION=0
		# if var_num is 0 means didn't found a bug, which_reduction only two type(hash variable or lines)
		if [ $WHICH_REDUCTION = 0 ]; then
			# go through func first
			COM_NUM="$(grep "Possible comment out" test_Mod.txt)" 
			COM_NUM=$(grep "Possible comment out" test_Mod.txt | grep -o '[0-9]*')
			COM_NUM=$(($COM_NUM - 0))
			echo "Possible comment out is $COM_NUM."
			ITERS=$COM_NUM
		else
			# if not found by looping through func then do variable
			HASH_NUM="$(grep "Total hash variable" test_Mod.txt)" 
			HASH_NUM=$(grep "Total hash variable" test_Mod.txt | grep -o '[0-9]*')
			HASH_NUM=$(($HASH_NUM - 0))
			echo "Total hash variable is $HASH_NUM."
			ITERS=$HASH_NUM
		fi
		# create a new file to save which variable went wrong
		touch problemVariable.txt
		for ((j = 1; j <= $ITERS; j++));
		do
			reduction_process $FUNC_NAME $j $WHICH_REDUCTION
			VAR_NUM=$(wc -l < problemVariable.txt)
			VAR_NUM=$(($VAR_NUM - 0))
			echo "$VAR_NUM problematic lines/variables in total. "
			cp test_reduced.c test_reduced.txt
			if [ $VAR_NUM = 0 ]; then 
				#didn't find problematic line through checking func, do variable check
				echo "Didn't find possible error line in $FUNC_NAME line $j, try next."
				CHECKED=1
				#cp test_modify.c test.txt
				#gcc removeRedundant.c -o removeRedundant &&./removeRedundant $FUNC_NAME $FUNC_NUM 1 >> removedLine.txt
			else
				REMOVED=1
				# error line
				# to reset the test.txt file for further redundant removing (current test.txt will have the last line commented so need to reset)
				cp test.c test.txt 
				for ((k = 1; k <= $VAR_NUM; k++));
				do
					# find problematic line, get to the line that is causing the problem
					LINE_NUM=$(head -$k problemVariable.txt| tail -1)
					LINE_NUM=1
					gcc modifyMain.c -o modifyMain &&./modifyMain 1 $FUNC_NAME $LINE_NUM > test_reduced.c
					#cp test_reduced.c test_reduced.txt
					# SO NEED TO DO FUNC NUM FOR EVERY PROGRAM
					if [ $k = $VAR_NUM ]; then 
						#remove extras
						gcc removeRedundant.c -o removeRedundant &&./removeRedundant $FUNC_NAME $FUNC_NUM 2 >> furtherFunc.txt
						break 3;
					else
						gcc removeRedundant.c -o removeRedundant &&./removeRedundant $FUNC_NAME $FUNC_NUM 1 >> furtherFunc.txt
					fi
					sort -V furtherFunc.txt -o furtherFunc.txt
				done
				> problemVariable.txt
			fi
			#cp test_removeRedundant.txt test.txt
			#cat test.txt > test.c
		done 

		FURTHER_FUNC=$(grep -c . furtherFunc.txt)
		FURTHER_FUNC=$(($FURTHER_FUNC - 0))
		echo "$FURTHER_FUNC function need further reduction."
		> problemVariable.txt #reset after current function is finished
		awk 1 furtherFunc.txt >> funcNameTmp.txt
		sort -uV funcNameTmp.txt -o funcNameTmp.txt
		> furtherFunc.txt #clean for next round
		LEFT_FUNC=$(wc -l < funcNameTmp.txt)
		LEFT_FUNC=$(($LEFT_FUNC - 0))
	done 
}

################################################################################
# Main function
################################################################################

read -p 'Please enter number of tests you would like to run: ' iterations
typeset -i END;
let END=iterations
for ((i = 1; i <= END; i++));
do 
# next line gets the program
        cd /media/sf_genTest1000/"$i"
# next line copy the test.c and turn into text for readability
	cp test.c /home/legup/legup-4.0/examples/tryCsmith
	cd /home/legup/legup-4.0/examples/tryCsmith
	echo "Test $i start... "
	cat test.c > test.txt
	# save a copy
	echo "Test $i orginal" >> testOrig.txt
	cat test.c >> testOrig.txt
# run the modify file so test is modified and number of loops is counted
# function names saved in functionName.txt
	touch functionName.txt
	gcc modifyMain.c -o modifyMain &&./modifyMain 0 func_1 0 > test_Mod.c
	LOOPNUM=0
	# randomly choose what directives to add
	# 0 means loop unroll, 1 means loop-level pipeline
	# 2 means 
	WHICH_DIRECTIVE=$(python -c "import random; print random.randint(0,4)")
#	WHICH_DIRECTIVE=1
	echo -n "Added directives label is : $WHICH_DIRECTIVE -> "
	# save a copy of modified
	echo "Test $i modified" >> testMod.txt
	cat test_Mod.c >> testMod.txt
	cat test_Mod.c > test_Mod.txt
	# infos
	FUNC_NUM=$(wc -l < functionName.txt)
	FUNC_NUM=$(($FUNC_NUM - 0))
	LOOPNUM="$(grep "Total number of" test_Mod.txt)" 
	LOOPNUM=$(grep "Total number of" test_Mod.txt | grep -o '[0-9]*')
	# save a flag if config file is modified 
	touch config.tcl
	LOCAL_CONFIG=0
	echo "source ../legup.tcl" > config.tcl
	# beginning of the Makefile
	touch Makefile
	echo "NAME=test_Mod" > Makefile
	if [ $WHICH_DIRECTIVE -eq 0 ]; then #unroll, Makefile parameter
		echo "Unrolling..."
		UNROLL_RANDOM=$(python -c "import random; print random.randint(1,3000)")
		echo "UNROLL = -unroll-allow-partial -unroll-threshold=$UNROLL_RANDOM" >> Makefile
	elif [ $WHICH_DIRECTIVE -eq 1 ]; then #pipeline, config parameter
		echo "Pipelining..."
	
		LOOP_RANDOM=$(($LOOPNUM - 0))
		echo "Total number of for-loops is $LOOP_RANDOM."
		IFPIPELINEALL=$(python -c "import random; print random.randint(1,100)")
		LOCAL_CONFIG=1
		if [ $IFPIPELINEALL -eq 1 ] && [ "$LOOP_RANDOM" -ne 0 ]; then
			echo "Pipeline all!"
			echo "set_parameter PIPELINE_ALL 1" >> config.tcl
		elif [ "$LOOP_RANDOM" -gt 1 ]; then #the range of random cannot be (1,1)
			LOOP_RANDOM=$(python -c "import random; print random.randint(1,$LOOP_RANDOM)")
			echo "Randomly choose $LOOP_RANDOM loops to perform pipeline."
			# directive files need to be changed
			for ((j = 1; j <= $LOOP_RANDOM; j++)); 
			do
				echo "loop_pipeline \"LOOP_$j\"" >> config.tcl
			done
		elif [ "$LOOP_RANDOM" -eq 1 ]; then 
			echo "Only $LOOP_RANDOM loop available to pipeline."
			echo "loop_pipeline \"LOOP_1\"" >> config.tcl
		else 
			echo "Don't pipeline!"
			echo "set_parameter NO_LOOP_PIPELINING 1" >> config.tcl
		fi
		# whether allow resource sharing
		IFSHARING=$(python -c "import random; print random.randint(1,2)")
		if [ $IFSHARING -eq 1 ]; then
			echo "set_parameter PIPELINE_RESOURCE_SHARING 1" >> config.tcl
		fi

	elif [ $WHICH_DIRECTIVE -eq 2 ]; then # no inline, Makefile parameter
		echo "No inline..."
		echo "NO_INLINE=1" >> Makefile
	elif [ $WHICH_DIRECTIVE -eq 3 ]; then # no optimization, Makefile parameter
		echo "No optimization..."
		echo "NO_OPT=1" >> Makefile
	elif [ $WHICH_DIRECTIVE -eq 4 ]; then # acclerate function, config file
		LOCAL_CONFIG=1
		echo  "Accelerating function..."
	
		echo -n "$FUNC_NUM functions in total. "
		if [ $FUNC_NUM -eq 0 ]; then
			echo "Don't accelerates!"
		elif [ $FUNC_NUM -gt 1 ]; then
			FUNC_RANDOM=$(python -c "import random; print random.randint(1,$FUNC_NUM)")
			echo "Randomly select $FUNC_RANDOM."
			for ((j = 1; j <= $FUNC_RANDOM; j++));
			do
				FUNC=$(head -$j functionName.txt | tail -1)
				echo "set_accelerator_function \"$FUNC\"" >> config.tcl
			done
		else
			FUNC=$(head -1 functionName.txt)
			echo "set_accelerator_function \"$FUNC\"" >> config.tcl
		fi
	fi
	# config file needs modification
	if [ $LOCAL_CONFIG -eq 1 ]; then
		echo "LOCAL_CONFIG= -legup-config=config.tcl" >> Makefile
	fi
	#end of the Makefile
	echo "LEVEL=.." >> Makefile
	echo "include \$(LEVEL)/Makefile.common" >> Makefile
# run to add loop labels
	gcc addDirective_c.c -o addDirective
	./addDirective $LOOPNUM > test_Mod.c
# run test_Mod.c using GCC to get the C result
	echo "Running C test through GCC.."
	gcc -m32 test_Mod.c -o test_Mod > warnings.txt
	timeout 5m ./test_Mod > goldenResult.txt
	C_TIME=$?
	SAVE=0
	echo $C_TIME
	# 136 is floating point exception
	if [ $C_TIME = 124 ] || [ $C_TIME = 136 ]; then
		echo "Test $i: No C output!" >> resultFile.txt
	else 
		echo "C output is generated!"
		# run make to produce the v file
		echo "Test $i log" > logfile.txt
		echo "Generating and Stimulating  Verilog files..."
		# make to get verilog files, then make v to run the verilog file
		timeout 120m make default  v > logfile.txt
		V_TIME=$?
		if [ $V_TIME = 124 ]; then
			echo "Test $i: No Verilog output!" >> resultFile.txt
			SAVE_NAME="noVresult_$i"
			SAVE=1
		elif [ $V_TIME = 2 ]; then
		    echo "Test $i: Stimulation problem!">> resultFile.txt
			SAVE_NAME="simProb_$i"
			SAVE=1
		else 
			echo "LegUp finished! Extracting result..."
			# run extractTrans to get the returned result 
			cp transcript transcript.txt
			gcc extractTrans.c -o extractTrans &&./extractTrans > verilogResult.txt
			echo "Test $i result: " >> resultFile.txt
			gcc resultCheck.c -o resultCheck &&./resultCheck >> resultFile.txt
			RESULT=$(tail -n2 resultFile.txt | head -1)
			if [[ "$RESULT" == *"Output does not"* ]]; then
				SAVE_NAME="error_$i"
				SAVE=2
				> problemVariable.txt
				reduction
			fi
		fi

		if [ $SAVE = 1 ] || [ $SAVE = 2 ]; then
		    
			mkdir /media/sf_legUp_err/0713/$SAVE_NAME
			cp test_Mod.c /media/sf_legUp_err/0713/$SAVE_NAME/test_Mod.c
			cp test.c  /media/sf_legUp_err/0713/$SAVE_NAME/test.c
			cp transcript  /media/sf_legUp_err/0713/$SAVE_NAME/logfile.txt
			cp goldenResult.txt /media/sf_legUp_err/0713/$SAVE_NAME/goldenResult.txt
			cp verilogResult.txt /media/sf_legUp_err/0713/$SAVE_NAME/verilogResult.txt
			cp config.tcl /media/sf_legUp_err/0713/$SAVE_NAME/config.tcl
			cp Makefile /media/sf_legUp_err/0713/$SAVE_NAME/Makefile
			if [ $SAVE = 2 ]; then
				cp test_removeRedundant.txt /media/sf_legUp_err/0713/$SAVE_NAME/test_removeRedundant.txt
				cp test_reduced.c /media/sf_legUp_err/0713/$SAVE_NAME/test_reduced.c
				cp test_ini.c /media/sf_legUp_err/0713/$SAVE_NAME/test_ini.c
				echo "$REMOVED $CHECKED"
					if [ $REMOVED = 1 ]; then
						echo "Sucessfully reduced!" > reductionRecord.txt
		
					elif [ $CHECKED = 1 ]; then
						echo "Reduction perfomed but unable to indicate problematic line within functions. Please check transparent_crc portion." > reductionRecord.txt
					else 
						echo "Reduction encountered trouble!" > reductionRecord.txt
					fi
					cp reductionRecord.txt /media/sf_legUp_err/0713/$SAVE_NAME/reductionRecord.txt
					> problemVariable.txt #reset after current function is finished
			fi
			SAVE=0
		fi   
	fi
	echo "Test $i finished!"
	# clear the configuration file and Makefile
#	> config.tcl
#	> Makefile
	> functionName.txt
done

# get the result
gcc grabResult.c -o grabResult &&./grabResult > grabResult.txt
echo
grep -e "performed" -e "failed" -e "Failed" -e "result" -e "produce" grabResult.txt

# go to folder and delete files
cd
cd /home/legup/legup-4.0/examples/tryCsmith 
read -p "Delete all test files and output files (y/n)?" ANS
if [ "$ANS" = "y" ]; then
 echo "Deleting...";
  rm -f testMod.txt testOrig.txt test.txt test.c test_Mod.c logfile.txt
  rm -f goldenResult.txt resultFile.txt grabResult.txt dfg* fsm*
  rm -f func_* g_* gantt* legup_* str* *rpt *ver *mif
 echo "Finished!"
else
 echo "Don't delete!"
fi

