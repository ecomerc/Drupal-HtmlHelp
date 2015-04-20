<?php



function htmlhelp_view() {

  $args = func_get_args();
  
  
	//TODO: LOAD THIS FROM SETTINGS
    switch ($args[0]) {
        case 8:
            $version = "orgcon8";
            $v = "8";
            array_shift($args);
            break;
        case 9:
            $version = "orgcon9";
            $v = "9";
            array_shift($args);
            break;
        default:
            $version = "orgcon9";
            $v = "9";
            break;
    }

    $file = implode($args, "/");
    
	//TODO: LOAD THIS FROM SETTINGS
    drupal_set_title(check_plain("Help " . $v));
    
    
    if ($file == '' || $file == 'index') {
        $output = _htmlhelp_load_index($file, $v, $version);
    } else {
        $breadcrumb = drupal_get_breadcrumb();
        $breadcrumb[] = l(drupal_get_title(), 'help/'.$v);
        drupal_set_breadcrumb($breadcrumb);
        $filedir = "";
        $url = _htmlhelp_clean_url($file, $v, $version, $filedir);
        $output = _htmlhelp_load_page($url, $v, $version, $title, $filedir);
        drupal_set_title($title);
    }
    if ($output == null) {
        drupal_not_found();
        die();
    }
    if ($_GET["win"] == "sm") {
        echo theme('htmlhelp_small', $output); //$page;
        die();
    } else {
        return $output;
    }
}

function _htmlhelp_urlreplace_callback($matches, $v, $filedir) {
    $pos = strpos($matches[2], "..");
    $args = array();
    if ($matches[3] != "") {
        $query_string = explode( '&', $matches[3] );
        $args = array( ); // return array
        foreach( $query_string as $chunk ) {
            $chunk = explode( '=', $chunk );
            if ( count( $chunk ) == 2 ) { 
                list( $key, $val ) = $chunk;
                $args["query"][ $key ] = urldecode( $val );
            }
        }
    }
    if (!empty($_GET["win"])) {
        $args["query"]["win"] = $_GET["win"];
    }
    if ($pos === false) {
        $url = str_replace("\\\\",  "/", $matches[2]);
        $url = str_replace("\\",  "/", $url);
        if (substr($url, 0,1) == "/") {
            $url = "help/".$v.$url;
        } else {
            $url = "help/".$v."/".$filedir.$url;
        }
        return url($url, $args);
    } else {
        return url(_htmlhelp_clean_url_inline($filedir,$matches[2], $v), $args);
    }
}


function _htmlhelp_load_index($file, $v, $version) {
	
	//TODO: LOAD THIS FROM SETTINGS
    $drupal_path = dirname($_SERVER['SCRIPT_FILENAME']);
    $filename =  $drupal_path ."/". $version."/help/OrgCon help.hhc";
    $path = drupal_get_path('module', 'htmlhelp');

    if (file_exists($filename)) {
      
        $fd = fopen ($filename, "r");
        $contents = fread ($fd, filesize ($filename));
        fclose ($fd);

        $body = $contents;


        $body = preg_replace("/<param name=\"Name\" value=\"([^\"]*)\">\s+(?:<param[^>]*>\s+)?<param name=\"Local\" value=\"((?:http|ftp):\/\/[^\"]*)\">/i", "<a href=\"\\2\">\\1</a>", $body);
        $body = preg_replace_callback(
            "/<param name=\"Name\" value=\"([^\"]*)\">\s+(?:<param[^>]*>\s+)?<param name=\"Local\" value=\"([^\"]*)\">/i",
            create_function(
                '$matches',
                'return "<a href=\"" . _htmlhelp_urlreplace_callback($matches, "' . $v . '","") . "\">" . $matches[1] . "</a>";'
            ),
            $body
        );
        //"<a href=\"" .  ."\">".$matches[1]."</a>";
        // .base_path()."help/".$v."/\\2?win=".$_GET["win"]."\">\\1</a>
        //$body = preg_replace("/<param name=\"Name\" value=\"([^\"]*)\">\s+(?:<param[^>]*>\s+)?<param name=\"Local\" value=\"([^\"]*)\">/i", "<a href=\"".base_path()."help/".$v."/\\2?win=".$_GET["win"]."\">\\1</a>", $body);
        $body = preg_replace("/<param((\s+[^\"'=]+)=\"([^\"]*)\"|('[^\']*')|([^\s\"'>]*)?)*\s*\/?>/i", "\\3", $body);

        $body = strip_tags($body, '<b><ul><li><a>');
        $body = str_replace_once("<UL>", "<ul id=\"myMenu\">", $body);

        
        drupal_add_js(drupal_get_path('module', 'htmlhelp') .'/htmlhelp.js');

        //$body .= "<script type=\"text/javascript\" src=\"$htmlhelp_js\"></script>\n";
        //$header .= "<style>.hiddenbullet {list-style-type:none;margin-left:30px;}</style>";
        
        $header = "";
        if ($v == 9) {
            $form_state = "";
            //$block = search_form($form_state, '','', "htmlhelp", "Query: ") ;
            $header = drupal_get_form('search_form', NULL, "", 'htmlhelp', "Search the help file"); 
            $header .= "<h2>Index</h2>";
            //$header .= $block['content']; 
        }
 
        
        $header .= "<p>Click the green arrows to expand the help index</p>";
        
        return theme('htmlhelp_page',  $header.$body);
    } else {
        return null;
    }
}

function _htmlhelp_clean_url($file, $v, $version, &$filedir = "") {
    $drupal_path = dirname($_SERVER['SCRIPT_FILENAME']);
    
    $file2 = str_replace("\\\\",  "/", $file);
    $file2 = str_replace("\\",  "/", $file2);
    $filedirs = explode("/", $file2);

    $i = 0 ;
    while ($i < (sizeof($filedirs)-1)) {
        if($filedirs[$i] == ".."){
            if($filedirs[$i-1] != ""){
                $filedirs[$i-1] = "";
            } else {
                $filedirs[$i-3] = "";
            }
            $filedirs[$i] = "";
        }
        $i++;
    }

    $i = 0 ;
    $filedir = "";
    while ($i < (sizeof($filedirs)-1)) {
        if($filedirs[$i] != ""){
            $filedir = $filedir.$filedirs[$i] . "/";
        }
        $i++;
    }

    $file =$filedir.$filedirs[(sizeof($filedirs)-1)];
    return ($drupal_path . "/" . $version . "/help/".$file);

}

function _htmlhelp_clean_url_inline($currentfilepath, $file, $v) {
    $file = $currentfilepath . $file;
    $file2 = str_replace("\\\\",  "/", $file);
    $file2 = str_replace("\\",  "/", $file2);
    $filedirs = explode("/", $file2);

    $i = 0 ;
    while ($i < (sizeof($filedirs)-1)) {
        if($filedirs[$i] == ".."){
            if($filedirs[$i-1] != ""){
                $filedirs[$i-1] = "";
            } else {
                $filedirs[$i-3] = "";
            }
            $filedirs[$i] = "";
        }
        $i++;
    }

    $i = 0 ;
    $filedir = "";
    while ($i < (sizeof($filedirs)-1)) {
        if($filedirs[$i] != ""){
            $filedir = $filedir.$filedirs[$i] . "/";
        }
        $i++;
    }

    $file =$filedir.$filedirs[(sizeof($filedirs)-1)];
    return ( "help/". $v . "/".$file);

}


function _htmlhelp_load_page($filename, $v, $version, &$title, $filedir = "") {

    if (file_exists($filename)) {
      

        $fd = fopen ($filename, "r");
            $contents = fread ($fd, filesize ($filename));
        fclose ($fd);


        preg_match("/<body[^>]*>([\w|\t|\r|\W]*)<\/body>/i", $contents, $search);
        $body = $search[1];
        //eregi(ereg_MatchedHTMLTags('title'), $contents, $search);
        preg_match("/<h1[^>]*>([\w|\t|\r|\W]*)<\/h1>/i", $contents, $search);
        if (empty($search[1])) {
          preg_match("/<title[^>]*>([\w|\t|\r|\W]*)<\/title>/i", $contents, $search);
        }
        //$title="Help";
        $mtitle = strip_tags($search[1]);
        $title = check_plain($mtitle);
        $body = strip_tags($body, '<table><br><u><td><tr><h1><h2><h3><img><param><li><ul><ol><p><a><div><b><strong>');
        
        $replacements['æ'] = '&aegir;';
        $replacements['ø'] = '&oslash;';
        $replacements['å'] = '%aring;';
        $replacements['·'] = "'";
        $replacements["\n"] = " "; //\n
        $replacements[chr(9)] = " "; //LF
        $replacements[chr(10)] = " "; //LF
        $replacements[chr(13)] = " "; //CR
        $replacements["   "] = " "; //CR
        $replacements["  "] = " "; //CR
        $replacements["®"] = "&copy;"; //CR
        $replacements[chr(10)] = " "; //LF
        $replacements['src="'] = 'src="'.base_path().$version.'/help/'.$filedir; //SRC
        $replacements['SRC="'] = 'SRC="'.base_path().$version.'/help/'.$filedir; //SRC
        $replacements['PARAM NAME=movie VALUE="'] = 'PARAM NAME=movie VALUE="'.base_path().$version.'/help/'.$filedir; 
        
        $body = strtr($body,$replacements);
        
        $body = preg_replace("/<h1>.*?<\/h1>/i", "", $body, 1);
        $body = preg_replace_callback(
            "/href[ ]*=[ ]*('|\")(?!http:\/\/)([^\"'#]*)(#?[^\"']*)('|\")/i",
            create_function(
                // single quotes are essential here,
                // or alternative escape all $ as \$
                '$matches',
                'return "href=\""._htmlhelp_urlreplace_callback($matches, "' . $v . '","' . $filedir . '")."\"";'
            ),
            $body
        );
        
        //$body = preg_replace("/href[ ]*=[ ]*('|\")(?!http:\/\/)([^\"'#]*)(#?[^\"']*)('|\")/i", "href=\"". base_path() ."help/".$v."/".$filedir."\\2?win=".$_GET["win"]."\\3\"", $body);
        //$body = str_replace("%23", "#", $body); //#

        $header = '<div class="linkbox">';
        $header .= '<ul>';
        $header .= '    <li class="download"><a href="'. base_path() .'help/'.$v.'/">Help Index</a></li>';
        $header .= '</ul>';
        $header .= '</div>';

        $header = "<h1>".check_plain($title)."</h1>";
        
        return theme('htmlhelp_page', $body);
    } else {
        return null;
    }
}


function template_preprocess_htmlhelp_page(&$variables) {
    
}
function template_preprocess_htmlhelp_pagesm(&$variables) {
    
}


function ereg_MatchedHTMLTags($tagname) {
// Use with eregi to ensure case-insensitive match.
//        e.g. to split an HTML page based on body tag:
//            eregi(ereg_MatchedHTMLTags('body'), $Source, $Matches)

// The following values will be held in $Matches
//(marked values are unintended byproducts of the expression)
//          *[0] - the entire string ($Source).
//            [1] - everything before the opening tag
//            [2] - the opening tag, including all contents (i.e. everything between < and >)
//          *[3] - the opening tag from end of the tag name,
//                      e.g. '<body bgcolor="#000000">' gives ' bgcolor="#000000">'
//            [4] - the tag contents (everything between the opening and closing tag)
//            [5] - the complete closing tag.
//          *[6] - the closing tag from the end of the tag name
//                      e.g. '</body invalid text>' gives ' invalid text>'
//            [7] - everything after the closing tag.

   return "^(.*)(<[ \\n\\r\\t]*$tagname(>|[^>]*>))(.*)(<[ \\n\\r\\t]*/[ \\n\\r\\t]*$tagname(>|[^>]*>))(.*)$";
}

function str_replace_once($needle, $replace, $haystack) { 
   // Looks for the first occurence of $needle in $haystack 
   // and replaces it with $replace. 
   $pos = strpos($haystack, $needle); 
   if ($pos === false) { 
       // Nothing found 
       return $haystack; 
   } 
   return substr_replace($haystack, $replace, $pos, strlen($needle)); 
}


