#!/bin/bash
# My first script
# can add PATH=$PATH:/cygdrive/c/Users/chang/OneDrive/Desktop if it is in # virtual box

echo "Welcome to HLS Fuzz testing - Vivado HLS!"
source /cygdrive/c/Xilinx/Vivado/2019.2/settings64.sh
CSMITHSRC="C:\csmith-2.3.0\src"
CSMITHRUNTIME="C:\csmith-2.3.0\runtime"
################################################################################
# Helper Option                                                                #
################################################################################
Help()
{
	# Display Helper functions
   echo "HLS Fuzzer command line options:"
   echo
   echo " -h   			  :print help information."
   echo " -p <options>     :probability file for Csmith random program generation."
   echo "    <options>: "
   echo "   display: display current Csmith probability setting."
   echo "   dump <filename>: dump current Csmith probability file to the filename specified."
   echo ""
}


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
	gcc modifyMain.c -o modifyMain &&./modifyMain 0 $FUNC_NAME $j > test_modify_main.c
	cat test_modify_main.c > test_modify_main.txt
	# use the same directive file then process the c code
	cp test_modify_main.txt test_modify.c
	# get the new golden result for reduced program
	gcc test_reduced.c -o test_reduced 
	timeout 5m ./test_reduced > out.gold.txt
	# feed to vivado
	timeout 120m vivado_hls -f testVivado.tcl >> logR.txt
	VAR_RE=$?
	echo "Vivado $j finished.."
	if [ $VAR_RE = 0 ]; then # passed means this variable is causing previous failure - save the hash variable
		echo "Problem found on $j"
		echo "$j" >> problemVariable.txt
	fi
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
	gcc modifyMain.c -o modifyMain &&./modifyMain 0 func_1 1 > test_modify_main.c
	cp test_modify_main.c test_modify_main.txt
	cp test_modify_main.c test_modify.c
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
		gcc modifyMain.c -o modifyMain &&./modifyMain 0 $FUNC_NAME 1 > test_modify_main.c
		cat test_modify_main.c > test_modify_main.txt
		VAR_NUM=0
		WHICH_REDUCTION=0
		# if var_num is 0 means didn't found a bug, which_reduction only two type(hash variable or lines)
		if [ $WHICH_REDUCTION = 0 ]; then
			# go through func first
			COM_NUM="$(grep "Possible comment out" test_modify_main.txt)" 
			COM_NUM=$(grep "Possible comment out" test_modify_main.txt | grep -o '[0-9]*')
			COM_NUM=$(($COM_NUM - 0))
			echo "Possible comment out is $COM_NUM."
			ITERS=$COM_NUM
		else
			# if not found by looping through func then do variable
			HASH_NUM="$(grep "Total hash variable" test_modify_main.txt)" 
			HASH_NUM=$(grep "Total hash variable" test_modify_main.txt | grep -o '[0-9]*')
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
# Check options                                                                #
################################################################################
while getopts ":hp" option; do
   case $option in
   		h) # display Help
        	Help
        	exit 0;;
	  	p) # probability
		  	echo "Csmith probability:"
			subcommand=$2; 
			echo $subcommand
			case "$subcommand" in
				display)
					cd $CSMITHSRC && cat prob2.txt
					exit 0
					;;
				dump)
					shift
					savefile=$2
					echo $savefile
					cd $CSMITHSRC && cp prob2.txt $savefile.txt
					exit 0
					;;
				\? )
					echo "Invalid Option: -$OPTARG" 1>&2
					exit 1
					;;
				: )
					echo "Invalid Option: -$OPTARG requires an argument" 1>&2
					exit 1
					;;
			esac
			#shift $((OPTIND -1))
			exit 0;;
     	\?) # incorrect option
         	echo "Error: Invalid option"
         	exit 1;;
   esac
done
shift $((OPTIND -1))





################################################################################
# Main program                                                                 #
################################################################################
read -p 'Please enter number of tests you would like to run: ' iterations
typeset -i END;
let END=iterations
for ((i = 1; i <= END; i++));
do 
# get into the pre-generated folder
	if [ $i -ne 1 ]; then
		PREV=$(($i-1))
		rm -rf add_"$PREV"
	fi 
	cd "C:\csmith-2.3.0\runtime\genTest10_1000/$i"
# next line copy the test.c and turn into text for readability
	cp test.c $CSMITHRUNTIME
	cd $CSMITHRUNTIME 
	echo "Test $i" >> testOriginal.txt
	cat test.c >> testOriginal.txt
	cat test.c > test.txt
# run the modify file so new test is generated
# save function names to a file for later adding directives use
	touch functionName.txt
	gcc modifyMain.c -o modifyMain &&./modifyMain 0 func_1 0 > test_modify_main.c
	cat test_modify_main.c > test_modify_main.txt
	cp directives.txt directives.tcl
	# randomly choose what directives to add
	# 0 means loop level optimization, 1 means function-level pipeline, 2 means array map,
	# 3 means array partition, 4 is array reshape
	WHICH_DIRECTIVE=$(python -c "import random; print random.randint(0,4)")
	echo -n "Added directives label is : $WHICH_DIRECTIVE -> "
	FUNC_NUM=$(wc -l < functionName.txt)
	FUNC_NUM=$(($FUNC_NUM - 0))
	gcc func_loop.c -o func_loop &&./func_loop $FUNC_NUM 
	LOOPNUM="$(grep "Total number of" test_modify_main.txt)" 
	LOOPNUM=$(grep "Total number of" test_modify_main.txt | grep -o '[0-9]*')
	LOOP_RANDOM=$(($LOOPNUM - 0))
	if [ $WHICH_DIRECTIVE -eq 0 ]; then 
		echo "Total number of for-loops is $LOOP_RANDOM."
		WHICH_DIRECTIVE=$(python -c "import random; print random.randint(0,1)")
		if [ $WHICH_DIRECTIVE -eq 0 ]; then
			echo "Unrolling..."
			DIRECTIVE_NAME="unroll"
		elif [ $WHICH_DIRECTIVE -eq 1 ]; then
			echo "Loop-Level Pipelining..."
			DIRECTIVE_NAME="pipeline"
		fi
		LOOPNUM="$(grep "Total number of" test_modify_main.txt)" 
		#echo "$LOOPNUM"
		LOOPNUM=$(grep "Total number of" test_modify_main.txt | grep -o '[0-9]*')
		LOOP_RANDOM=$(($LOOPNUM - 0))
		echo "Total number of for-loops is $LOOP_RANDOM."
		REWIND=$(python -c "import random; print random.randint(1,2)")
		if [ "$LOOP_RANDOM" -gt 1 ]; then 
		#the range of random cannot be (1,1)
			LOOP_RANDOM=$(python -c "import random; print random.randint(1,$LOOP_RANDOM)")
			echo "Randomly choose $LOOP_RANDOM loops to perform $DIRECTIVE_NAME."
			# directive files need to be changed
			for ((j = 1; j <= $LOOP_RANDOM; j++)); 
			do
				if [ $WHICH_DIRECTIVE -eq 1 ] && [ $REWIND -eq 2 ]; then
					echo "set_directive_$DIRECTIVE_NAME -rewind \"test_modify/LOOP_$j\"" >> directives.tcl
				else 
					echo "set_directive_$DIRECTIVE_NAME \"test_modify/LOOP_$j\"" >> directives.tcl
				fi
			done
		elif [ "$LOOP_RANDOM" -eq 1 ]; then 
			echo "Only $LOOP_RANDOM loop available to $DIRECTIVE_NAME."
			if [ $WHICH_DIRECTIVE -eq 1 ] && [ $REWIND -eq 2 ]; then
				echo "set_directive_$DIRECTIVE_NAME -rewind \"test_modify/LOOP_1\"" >> directives.tcl
			else 
				echo "set_directive_$DIRECTIVE_NAME \"test_modify/LOOP_1\"" >> directives.tcl
			fi
		else 
			echo "Don't apply $DIRECTIVE_NAME"
		fi
		WHICH_DIRECTIVE=0
		
	elif [ $WHICH_DIRECTIVE -eq 1 ]; then  #function-level 
		echo "Function-Level Pipelining..." 
		DIRECTIVE_NAME="pipeline"
		echo -n "$FUNC_NUM functions in total. "
		FLUSH=$(python -c "import random; print random.randint(1,2)")
		if [ $FUNC_NUM -eq 0 ]; then
			echo "No Functions!"
		else
			if [ $FUNC_NUM -gt 1 ]; then
				FUNC_RANDOM=$(python -c "import random; print random.randint(1,$FUNC_NUM)")
			else 
				FUNC_RANDOM=1
			fi
			echo "Randomly select $FUNC_RANDOM."
			for ((j = 1; j <= $FUNC_RANDOM; j++));
			do
				FUNC=$(head -$j functionName.txt| tail -1)
				# func_opt: 1 function-level pipeline, 2 function-level loop merge, 3 function-level inline
				# 4 expression_balance
				FUNC_OPT=$(python -c "import random; print random.randint(1,5)")
				if [ $FUNC_OPT -eq 1 ] && [ $FLUSH -eq 1 ]; then #don't enable flush
					echo "set_directive_pipeline \"$FUNC\"" >> directives.tcl
				elif [ $FUNC_OPT -eq 1 ]; then
					echo "set_directive_$DIRECTIVE_NAME -enable_flush \"$FUNC\"" >> directives.tcl
				elif [ $FUNC_OPT -eq 2 ]; then
					echo "set_directive_loop_merge \"$FUNC\"" >> directives.tcl
				elif [ $FUNC_OPT -eq 3 ]; then
					echo "set_directive_inline -region \"$FUNC\"" >> directives.tcl
				elif [ $FUNC_OPT -eq 4 ]; then
					echo "set_directive_expression_balance  \"$FUNC\"" >> directives.tcl
				fi
			done
		fi
	elif [ $WHICH_DIRECTIVE -eq 2 ] || [ $WHICH_DIRECTIVE -eq 3 ] || [ $WHICH_DIRECTIVE -eq 4 ]; then  #array map is 2, array partition is 3, array_reshape is 4
		sort -u arrayVariables.txt -o arrayVariables.txt
		if [ $WHICH_DIRECTIVE -eq 3 ]; then
			echo "Array partition..."
			DIRECTIVE_NAME="partition"
		elif [ $WHICH_DIRECTIVE -eq 4 ]; then
			echo "Array reshape..."
			DIRECTIVE_NAME="reshape"
		else 
			echo "Array map..."
			DIRECTIVE_NAME="map"
		fi
		ARRAY_NUM=$(wc -l < arrayVariables.txt)
		ARRAY_NUM=$(($ARRAY_NUM - 0))
		if [ $ARRAY_NUM -eq 0 ]; then
			echo "No array-type variables!"
		else
			if [ $ARRAY_NUM -gt 1 ]; then
				ARRAY_RANDOM=$(python -c "import random; print random.randint(1,$ARRAY_NUM)")
			else
				ARRAY_RANDOM=1
			fi
			echo "Randomly select $ARRAY_RANDOM."
			for ((j = 1; j <= $ARRAY_RANDOM; j++));
			do
				ARRAY=$(head -$j arrayVariables.txt| tail -1)
				if [ $WHICH_DIRECTIVE -eq 2 ]; then
					vOrH=$(python -c "import random; print random.randint(1,2)")
					if [ $vOrH -eq 1 ]; then
						echo "set_directive_array_$DIRECTIVE_NAME -mode vertical \"func_1\" $ARRAY" >> directives.tcl
					else
						echo "set_directive_array_$DIRECTIVE_NAME -mode horizontal \"func_1\" $ARRAY" >> directives.tcl
					fi
				else 
					echo "set_directive_array_$DIRECTIVE_NAME -type complete \"func_1\" $ARRAY" >> directives.tcl
				fi

			done
		fi
	else 
		echo "No directives..."

	fi
 
	if [ $WHICH_DIRECTIVE -eq 0 ] || [ $WHICH_DIRECTIVE -eq 1 ]; then 
		gcc addDirective_c.c -o addDirective_c &&./addDirective_c $LOOP_RANDOM > test_modify.c
	else 
		cp test_modify_main.c test_modify.c
	fi
	echo "Modified Test $i" >> testModified.txt
	cat test_modify.c >> testModified.txt
	gcc test.c -o gold > log.txt
	# next line ensures that the process doesn't stuck there forever
	# the exit status will be checked to see if it runs successful 
	timeout 5m ./gold > out.gold.txt
	VAR=$?
	echo "C exit status is" $VAR >> resultFile.txt
	#tail resultFile.txt
	[ -s out.gold.txt ] && echo "C output is generated!" || VAR=124
	if [ $VAR = 124 ]; then
		echo "C does not produce an output!"
		echo "Test $i: No C output!" >> resultFile.txt
		((END++))
	else 
		echo "Entering Vivado HLS..."
		echo "Test $i log" >> testsLog.txt
		# creating tcl file for running vivado hls
		cat tclHeader.txt > testVivado.tcl
		echo "" >> testVivado.tcl
		echo "open_project -reset add_$i" >> testVivado.tcl
		cat tclComp.txt >> testVivado.tcl
		timeout 120m vivado_hls -f testVivado.tcl >> testsLog.txt
		VAR1=$?
		echo "Log file generated! Vivado exit status is " $VAR1
		#if [ $VAR1 = 124 ] || [ $VAR1 = 1 ] then Vivado either didn't finish or produced a wrong result
		echo "Vivado hls finished! Entering result extracting..."
		cd $CSMITHRUNTIME
		echo "Test $i: " >> resultFile.txt
		gcc resultExtract.c -o resultExtract &&./resultExtract >> resultFile.txt
		if grep -e "pragma" vivado_hls.log
		then
			echo "Wrong Pragma ignored!"
			echo "Wrong Pragma usage" >> resultFile.txt
			((END++))
			mkdir err_0716/prg_"$i"
			cp test_modify.c  err_0716/prg_"$i"/test_modify_"$i".c
			cp test.c  err_0716/prg_"$i"/test_"$i".c
			cp vivado_hls.log  err_0716/prg_"$i"/vivado_hls_"$i".txt
			cp out.gold.txt err_0716/prg_"$i"/out.gold.txt
			cp directives.tcl err_0716/prg_"$i"/directives.tcl
		else
			# save code that causing errors
			if [ $VAR1 = 1 ]; then
				echo "Test $i fails!"
				VAR2="errortest_$i"
			elif [ $VAR1 = 124 ]; then
				echo "Test $i didn't produce an output!"
				VAR2="crashtest_$i"
			fi
			
			if [ $VAR1 = 1 ] || [ $VAR1 = 124 ]; then
				mkdir err_0716/$VAR2
				cp test_modify.c  err_0716/$VAR2/test_modify_"$i".c
				cp test.c  err_0716/$VAR2/test_"$i".c
				cp vivado_hls.log  err_0716/$VAR2/vivado_hls_"$i".txt
				cp out.gold.txt err_0716/$VAR2/out.gold.txt
				cp directives.tcl err_0716/$VAR2/directives.tcl

				if [ $VAR1 = 1 ]; then
					# bug found go to reduction
					reduction
					cp test_removeRedundant.txt err_0716/$VAR2/test_removeRedundant_"$i".txt
					cp test_reduced.c err_0716/$VAR2/test_reduced_"$i".c
					cp test_ini.c err_0716/$VAR2/test_ini.c
					cp logR.txt err_0716/$VAR2/logR.txt
					if [ $REMOVED = 1 ]; then
						echo "Sucessfully reduced!" > reductionRecord.txt
		
					elif [ $CHECKED = 1 ]; then
						echo "Reduction perfomed but unable to indicate problematic line within functions. Please check transparent_crc portion." > reductionRecord.txt
					else 
						echo "Reduction encountered trouble!" > reductionRecord.txt
					fi
					cp reductionRecord.txt err_0716/$VAR2/reductionRecord.txt
					> problemVariable.txt #reset after current function is finished
				fi
			fi

			
		fi 
		
	fi
	echo "Test $i finished!"
# vivado_hls -i this is line by line tcl script
done

gcc checkResult.c -o checkResult &&./checkResult > resultCheck.txt
echo
grep -e "performed" -e "error" -e "produce" resultCheck.txt


read -p "Delete all test files and output files (y/n)? " ANS
if [ "$ANS" = "y" ]; then
echo "Deleting...";
	rm -f test.txt test.c test_modify.c out.gold.txt testModified.txt testOriginal.txt
	rm -f testsLog.txt resultFile.txt test_modify_main.c
	rm -f test_modify_main.txt resultCheck.txt
	rm -f checkResult.exe resultExtract.exe gold.exe
	rm -f addDirective_c.exe modifyMain.exe
	rm -f test_reduced.c test_need_reduce.txt problemVariable.txt
	rm -f log.txt
	cp directives.txt directives.tcl
echo "Finished!" 
else
echo "Don't delete!"
fi
