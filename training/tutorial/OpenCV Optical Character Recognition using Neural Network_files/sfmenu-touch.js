




/*
     FILE ARCHIVED ON 20:22:54 Jan 13, 2015 AND RETRIEVED FROM THE
     INTERNET ARCHIVE ON 23:38:16 Feb 23, 2016.
     JAVASCRIPT APPENDED BY WAYBACK MACHINE, COPYRIGHT INTERNET ARCHIVE.

     ALL OTHER CONTENT MAY ALSO BE PROTECTED BY COPYRIGHT (17 U.S.C.
     SECTION 108(a)(3)).
*/
/*
 * sf-Touchscreen v1.0b - Provides touchscreen compatibility for the jQuery Superfish plugin. - LAST UPDATE: MARCH 23rd, 2011
 *
 * Developer's notes:
 * Built as a part of the Superfish project for Drupal (/web/20150113202254/http://drupal.org/project/superfish) 
 * Found any bug? have any cool ideas? contact me right away! /web/20150113202254/http://drupal.org/user/619294/contact
 *
 * jQuery version: 1.3.x or higher.
 *
 * Dual licensed under the MIT and GPL licenses:
 *   /web/20150113202254/http://www.opensource.org/licenses/mit-license.php
 *   /web/20150113202254/http://www.gnu.org/licenses/gpl.html
*/

(function($){
  $.fn.sftouchscreen = function() {
    // Return original object to support chaining.
    return this.each( function() {
      // Select hyperlinks from parent menu items.
      $(this).find('li > ul').closest('li').children('a').each( function() {
     var $item = $(this);
        // No .toggle() here as it's not possible to reset it.
        $item.click( function(event){
       // Already clicked? proceed to the URI.
          if ($item.hasClass('sf-clicked')) {
            var $uri = $item.attr('href');
            window.location = $uri;
          }
          else {
            event.preventDefault();
            $item.addClass('sf-clicked');
          }
        }).closest('li').mouseleave( function(){
          // So, we reset everything.
          $item.removeClass('sf-clicked');
        });
   });
    });
  };
})(jQuery);