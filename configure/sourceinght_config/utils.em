/* Utils.em - a small collection of useful editing macros */



/*-------------------------------------------------------------------------
	I N S E R T   H E A D E R

	Inserts a comment header block at the top of the current function. 
	This actually works on any type of symbol, not just functions.

	To use this, define an environment variable "MYNAME" and set it
	to your email name.  eg. set MYNAME=raygr
-------------------------------------------------------------------------*/
macro InsertHeader()
{
	// Get the owner's name from the environment variable: MYNAME.
	// If the variable doesn't exist, then the owner field is skipped.
	szMyName = getenv(MYNAME)
	
	// Get a handle to the current file buffer and the name
	// and location of the current symbol where the cursor is.
	hbuf = GetCurrentBuf()
	szFunc = GetCurSymbol()
	ln = GetSymbolLine(szFunc)

	// begin assembling the title string
	sz = "/*   "
	
	/* convert symbol name to T E X T   L I K E   T H I S */
	cch = strlen(szFunc)
	ich = 0
	while (ich < cch)
		{
		ch = szFunc[ich]
		if (ich > 0)
			if (isupper(ch))
				sz = cat(sz, "   ")
			else
				sz = cat(sz, " ")
		sz = Cat(sz, toupper(ch))
		ich = ich + 1
		}
	
	sz = Cat(sz, "   */")
	InsBufLine(hbuf, ln, sz)
	InsBufLine(hbuf, ln+1, "/*-------------------------------------------------------------------------")
	
	/* if owner variable exists, insert Owner: name */
	if (strlen(szMyName) > 0)
		{
		InsBufLine(hbuf, ln+2, "    Owner: @szMyName@")
		InsBufLine(hbuf, ln+3, " ")
		ln = ln + 4
		}
	else
		ln = ln + 2
	
	InsBufLine(hbuf, ln,   "    ") // provide an indent already
	InsBufLine(hbuf, ln+1, "-------------------------------------------------------------------------*/")
	
	// put the insertion point inside the header comment
	SetBufIns(hbuf, ln, 4)
}






/* InsertFileHeader:

   Inserts a comment header block at the top of the current function. 
   This actually works on any type of symbol, not just functions.

   To use this, define an environment variable "MYNAME" and set it
   to your email name.  eg. set MYNAME=raygr
*/
macro fileInsertFileHeader()
{
	szMyName = getenv(USERNAME)
	
	hbuf = GetCurrentBuf()  
    sfile = GetBufName(hbuf)
    line_num = 0

    InsBufLine(hbuf,line_num++ , "/******************************************************************")
	InsBufLine(hbuf,line_num++ , " * @@file  @sfile@")
	
	/* if owner variable exists, insert Owner: name */
	InsBufLine(hbuf, line_num++, " * @@brief ")
	if (strlen(szMyName) > 0)
	{
		sz = " * @@author: @szMyName@"
		InsBufLine(hbuf, line_num++, sz)
		create_time = GetSysTime(1)
		/*
		time_y = create_time.Year
		time_m = create_time.Month
		time_d = create_time.Day
		create_date = "@time_y@" # "/" # "@time_m@" # "/" # "@time_d@"
		*/
		create_date = create_time.date
		//InsBufLine(hbuf, line_num++, " * @@Date " # " " # time_y # "/" # time_m # "/" #time_d)
		InsBufLine(hbuf, line_num++, " * @@Date " # "@create_date@")
		InsBufLine(hbuf, line_num++, " * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. ")
		InsBufLine(hbuf, line_num++, " * Change_date" # "      " # "   Owner   " # "      " # "Change_content")
        InsBufLine(hbuf, line_num++, " * @create_date@" # "      " # "  @szMyName@ " # "      " # "create file")
        InsBufLine(hbuf,line_num++,"")
      
		
		InsBufLine(hbuf, line_num++, "*****************************************************************/")
	}
	else
	{
		msg("no environment variable:USERNAME,please set it for your PC ")
	}
	
}


// Inserts "Returns True .. or False..." at the current line
macro ReturnTrueOrFalse()
{
	hbuf = GetCurrentBuf()
	ln = GetBufLineCur(hbuf)

	InsBufLine(hbuf, ln, "    Returns True if successful or False if errors.")
}



/* Inserts ifdef REVIEW around the selection */
macro IfdefReview()
{
	IfdefSz("REVIEW");
}


/* Inserts ifdef BOGUS around the selection */
macro IfdefBogus()
{
	IfdefSz("BOGUS");
}


/* Inserts ifdef NEVER around the selection */
macro IfdefNever()
{
	IfdefSz("NEVER");
}


// Ask user for ifdef condition and wrap it around current
// selection.
macro InsertIfdef()
{
	sz = Ask("Enter ifdef condition:")
	if (sz != "")
		IfdefSz(sz);
}

macro InsertCPlusPlus()
{
	IfdefSz("__cplusplus");
}


// Wrap ifdef <sz> .. endif around the current selection
macro IfdefSz(sz)
{
	hwnd = GetCurrentWnd()
	lnFirst = GetWndSelLnFirst(hwnd)
	lnLast = GetWndSelLnLast(hwnd)
	 
	hbuf = GetCurrentBuf()
	InsBufLine(hbuf, lnFirst, "#ifdef @sz@")
	InsBufLine(hbuf, lnLast+2, "#endif /* @sz@ */")
}


// Delete the current line and appends it to the clipboard buffer
macro KillLine()
{
	hbufCur = GetCurrentBuf();
	lnCur = GetBufLnCur(hbufCur)
	hbufClip = GetBufHandle("Clipboard")
	AppendBufLine(hbufClip, GetBufLine(hbufCur, lnCur))
	DelBufLine(hbufCur, lnCur)
}


// Paste lines killed with KillLine (clipboard is emptied)
macro PasteKillLine()
{
	Paste
	EmptyBuf(GetBufHandle("Clipboard"))
}



// delete all lines in the buffer
macro EmptyBuf(hbuf)
{
	lnMax = GetBufLineCount(hbuf)
	while (lnMax > 0)
		{
		DelBufLine(hbuf, 0)
		lnMax = lnMax - 1
		}
}


// Ask the user for a symbol name, then jump to its declaration
macro JumpAnywhere()
{
	symbol = Ask("What declaration would you like to see?")
	JumpToSymbolDef(symbol)
}

	
// list all siblings of a user specified symbol
// A sibling is any other symbol declared in the same file.
macro OutputSiblingSymbols()
{
	symbol = Ask("What symbol would you like to list siblings for?")
	hbuf = ListAllSiblings(symbol)
	SetCurrentBuf(hbuf)
}


// Given a symbol name, open the file its declared in and 
// create a new output buffer listing all of the symbols declared
// in that file.  Returns the new buffer handle.
macro ListAllSiblings(symbol)
{
	loc = GetSymbolLocation(symbol)
	if (loc == "")
		{
		msg ("@symbol@ not found.")
		stop
		}
	
	hbufOutput = NewBuf("Results")
	
	hbuf = OpenBuf(loc.file)
	if (hbuf == 0)
		{
		msg ("Can't open file.")
		stop
		}
		
	isymMax = GetBufSymCount(hbuf)
	isym = 0;
	while (isym < isymMax)
		{
		AppendBufLine(hbufOutput, GetBufSymName(hbuf, isym))
		isym = isym + 1
		}

	CloseBuf(hbuf)
	
	return hbufOutput

}

//!create new document 
event DocumentNew(sfile)
{
    fileInsertFileHeader()
    len = strlen(sfile)
    if ((sfile[len-1] == "h") || (sfile[len-1] == "H"))
    {
		hbuf = GetCurrentBuf()  
        hfile_name = GetBufName(hbuf)

        hfile_name_2 = toupper(hfile_name)
        hfile_name_3= strtrunc(hfile_name_2,strlen(hfile_name_2)-2)
        
        pre_1 = "#ifndef " # "   " # hfile_name_3 # "_H" 
        pre_2 = "#ifdef " # "    " # hfile_name_3 # "_H" 
       
        post = "#endif" 
	    AppendBufLine(hbuf,pre_1)
	    AppendBufLine(hbuf,pre_2)

	    i = 0
        while(i < 5)
        {
           AppendBufLine(hbuf," ")
           i++
        }
	    AppendBufLine(hbuf,post)
	}
	else if ((sfile[len-1] == "c") || (sfile[len-1] == "C"  ))
	{
        hbuf = GetCurrentBuf()  
        AppendBufLine(hbuf,"/**************************include******************************/")
         i = 0
        while(i < 5)
        {
           AppendBufLine(hbuf," ")
           i++
        }
        
        AppendBufLine(hbuf,"/**************************function******************************/")
	}
	else
	{

	}
}

//！global 使用时必须在函数体内重新声明
global func_name 
global func_line 
global func_name_para 
global last_line_read    
global all_str = ""
function read_function_line(hbuf,line)
{


    global last_line_read
    
	s = GetBufLine(hbuf,line)
	str_len = strlen(s)

	if (str_len < 1)
	{
       return
	}

	// find ")"
	while(str_len > 0)  //如果找到了")"
	{
		if (s[--str_len] == ")")
		{
		   // msg("find )")
		    last_line_read = line  //!最后找到的行
		    return
	 		
		}
	}

	if (str_len == 0)
	{
		//msg("there is no ) in this line" # line)
		last_line_read = line +1
        read_function_line(hbuf,last_line_read)
	}


}

macro InsertFunctionHeader( )
{

    global func_name_para
    global last_line_read
    global all_str

    
    szMyName = getenv(USERNAME)
    create_time = GetSysTime(1)

    time_y = create_time.Year
	time_m = create_time.Month
	time_d = create_time.Day
	//create_date = "@time_y@" # "/" # "@time_m@" # "/" # "@time_d@"  //！格式为:2019/8/10
	create_date = create_time.date   //!格式为2019年8月10日
	hbuf = GetCurrentBuf()  

	
	


    all_str = "" //!每次都重新初始化
 	func_name=GetCurSymbol()

	if (func_name == "")
	{
		msg("there is no function founded")
	    return
	}


	symbol_line = GetSymbolLine(func_name)
    //msg(symbol_line)
    
	symbol_func =GetSymbolLocation(func_name) 
	//msg(symbol_func)
	


	/*计算函数参数*/

	//！step1:find the line space of function declaration 
	func_name_para.first_line = symbol_line; 
	
	read_function_line(hbuf,symbol_line)
	
	func_name_para.last_line = last_line_read 

   // msg("name_first_line:" # func_name_para.first_line)
   // msg("name_last_line:" # func_name_para.last_line)
    //!step2: read all line in the space 
    while(func_name_para.first_line <= func_name_para.last_line)
    {
        s = GetBufLine(hbuf,func_name_para.first_line)
        new_s = ""
        i = 0
        while(i<strlen(s))
        {
			if (s[i] != " ")
			{
                first_valid = i; 
                break; 
			}
			i= i+1
        }

        s_1 = strmid(s,first_valid,strlen(s)) //!去掉字符串头部的空格
        
        all_str = cat(all_str,s_1)
        func_name_para.first_line = func_name_para.first_line + 1
    }
	// msg(all_str)  //！打印函数声明

	//!step3: truncate the content in ( )
	param_num = 0
    param_list = SymListNew()

	process_txt_len  = strlen(all_str)
    i = 0 
    j = 0

    while(i < process_txt_len)
    {
        if(all_str[i] == "," || all_str[i] == ")")
        {
            j = i-1
            //msg("i = @i@")
            while(j > 0)
            {
                //！往前找一个,
                if( //all_str[j] == "*" || all_str[j] == "&" ||
                   all_str[j] == "(" || all_str[j] == ",") 
                {
                   //msg("j = @j@")
                    last_pos = i-1 //!从后往前找第一个不是空格的位置
                    while(last_pos > j)
                    {
                       if (all_str[last_pos] != " ")
                       {
                           break
                       }
                       last_pos = last_pos-1
                    } 

                    valid_last = last_pos+1
                   
                   // msg("valid_last = @valid_last@  ")

                     
                    first_pos = valid_last
                    while(first_pos > j)
                    {
                        if (all_str[first_pos] == " ")
                        {
                               break
                        }
                        first_pos = first_pos - 1
                    }

                    valid_first = first_pos
                  
                   // msg("valid_first = @valid_first@")
                   
                    
                    symbol_func.Symbol = strmid(all_str,valid_first,valid_last)
                    //msg(symbol_func.Symbol)  //!打印参数
                    SymListInsert(param_list,param_num,symbol_func)
                    param_num = param_num + 1
                    break;
                }
                j = j - 1
            }
        }
        
        i = i + 1
    }
    msg("function parameter number:@param_num@")


    //!生成header 	
	i=0
    while (i < param_num + 5)
    {
		InsBufLine(hbuf,symbol_line+i,"")
		i++
	}
	func_line = symbol_line + param_num + 5

	
    PutBufLine (hbuf, func_line -5-param_num, " /*!   ")
	PutBufLine (hbuf,  func_line -4 -param_num ," * @@brief:  ")
	PutBufLine (hbuf,  func_line -3 -param_num ," * @@author:  @szMyName@")
	PutBufLine (hbuf,  func_line -2 -param_num ," * @@Date:  @create_date@")
	param_index = 0
	while (param_index <param_num)
	{
	    symbol_rec=SymListItem(param_list,param_num - 1 - param_index)
	    param_line = " * @@param:"  # symbol_rec.Symbol  # " :"

	    space_num = 30- strlen(param_line)
	    while (space_num > 0)
	    {
            space_num = space_num -1
            param_line = cat(param_line, " ")
            
	    }
	    param_line = cat(param_line,"[param description ]")
		PutBufLine (hbuf,  func_line - 2 -param_index, param_line)
		param_index = param_index + 1
	}
	PutBufLine  (hbuf,func_line-1 , " */")
	
   // msg("insert function header successfully")

}

