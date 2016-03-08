




/*
     FILE ARCHIVED ON 12:20:26 Jan 13, 2015 AND RETRIEVED FROM THE
     INTERNET ARCHIVE ON 23:38:16 Feb 23, 2016.
     JAVASCRIPT APPENDED BY WAYBACK MACHINE, COPYRIGHT INTERNET ARCHIVE.

     ALL OTHER CONTENT MAY ALSO BE PROTECTED BY COPYRIGHT (17 U.S.C.
     SECTION 108(a)(3)).
*/
/**
 * jQuery Mobile Menu 
 * Turn unordered list menu into dropdown select menu
 * version 1.0(31-OCT-2011)
 * 
 * Built on top of the jQuery library
 *   /web/20150113122026/http://jquery.com
 * 
 * Documentation
 *   /web/20150113122026/http://github.com/mambows/mobilemenu
 */
(function($){
$.fn.mobileMenu = function(options) {
  
  // function isMobile(){
  // return ($('body').width() < 760);
 // }
 
 var defaults = {
   defaultText: 'Navigate to...',
   className: 'select-menu',
   subMenuClass: 'sub-menu',
   subMenuDash: '&ndash;'
  },
  settings = $.extend( defaults, options ),
  el = $(this);
 
 this.each(function(){
  // ad class to submenu list
  el.find('ul').addClass(settings.subMenuClass);

  // Create base menu
  $('<select />',{
   'class' : settings.className
  }).insertAfter( el );

  // Create default option
  $('<option />', {
   "value"  : '#',
   "text"  : settings.defaultText
  }).appendTo( '.' + settings.className );

  // Create select option from menu
  el.find('a,.separator').each(function(){
   var $this  = $(this),
     optText = $this.text(),
     optSub = $this.parents( '.' + settings.subMenuClass ),
     len   = optSub.length,
     dash;
   
   // if menu has sub menu
   if( $this.parents('ul').hasClass( settings.subMenuClass ) ) {
    dash = Array( len+1 ).join( settings.subMenuDash );
    optText = dash + optText;
   }
   if($this.is('span')){
    // Now build menu and append it
   $('<optgroup />', {
    "label" : optText,
   }).appendTo( '.' + settings.className );
   }
   else{
    // Now build menu and append it
   $('<option />', {
    "value" : this.href,
    "html" : optText,
    "selected" : (this.href == window.location.href)
   }).appendTo( '.' + settings.className );
   }

  }); // End el.find('a').each

  // Change event on select element
  $('.' + settings.className).change(function(){
   var locations = $(this).val();
   if( locations !== '#' ) {
    window.location.href = $(this).val();
   }
  });
  $('.select-menu').show();

 }); // End this.each
 // function runPlugin(){
  
  //menu exists, and browser is mobile width
  // if(isMobile()){
   // $('.select-menu').show();
   // $('.sf-menu').hide();
  // }
   
  // otherwise, hide the mobile menu
  // else{
   // $('.select-menu').hide();
   // $('.sf-menu').show();
  // }
  
 // }//runPlugin()
 
// runPlugin();
   // $(window).resize(function(){runPlugin();});

 return this;
};
})(jQuery);