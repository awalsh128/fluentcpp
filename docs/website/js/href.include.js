$(function() {
  $("[href-include]").each(function() {
    $(this).load($(this).attr('href-include'));
  });
})