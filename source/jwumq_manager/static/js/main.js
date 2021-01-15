jQuery(document).ready(function( $ ) {

  // Header fixed and Back to top button
  $(window).scroll(function() {
    if ($(this).scrollTop() > 100) {
      $('.back-to-top').fadeIn('slow');
      $('#header').addClass('header-fixed');
    } else {
      $('.back-to-top').fadeOut('slow');
      $('#header').removeClass('header-fixed');
    }
  });
  $('.back-to-top').click(function(){
    $('html, body').animate({scrollTop : 0},1500, 'easeInOutExpo');
    return false;
  });

  // Initiate the wowjs
  new WOW().init();

  // Initiate superfish on nav menu
  $('.nav-menu').superfish({
    animation: {opacity:'show'},
    speed: 400
  });

  // Mobile Navigation
  if( $('#nav-menu-container').length ) {
    let $mobile_nav = $('#nav-menu-container').clone().prop({ id: 'mobile-nav'});
    $mobile_nav.find('> ul').attr({ 'class' : '', 'id' : '' });
    $('body').append( $mobile_nav );
    $('body').prepend( '<button type="button" id="mobile-nav-toggle"><i class="fa fa-bars"></i></button>' );
    $('body').append( '<div id="mobile-body-overly"></div>' );
    $('#mobile-nav').find('.menu-has-children').prepend('<i class="fa fa-chevron-down"></i>');

    $(document).on('click', '.menu-has-children i', function(e){
      $(this).next().toggleClass('menu-item-active');
      $(this).nextAll('ul').eq(0).slideToggle();
      $(this).toggleClass("fa-chevron-up fa-chevron-down");
    });

    $(document).on('click', '#mobile-nav-toggle', function(e){
      $('body').toggleClass('mobile-nav-active');
      $('#mobile-nav-toggle i').toggleClass('fa-times fa-bars');
      $('#mobile-body-overly').toggle();
    });

    $(document).click(function (e) {
      let container = $("#mobile-nav, #mobile-nav-toggle");
      if (!container.is(e.target) && container.has(e.target).length === 0) {
       if ( $('body').hasClass('mobile-nav-active') ) {
          $('body').removeClass('mobile-nav-active');
          $('#mobile-nav-toggle i').toggleClass('fa-times fa-bars');
          $('#mobile-body-overly').fadeOut();
        }
      }
    });
  } else if ( $("#mobile-nav, #mobile-nav-toggle").length ) {
    $("#mobile-nav, #mobile-nav-toggle").hide();
  }

  // Smoth scroll on page hash links
  $('a[href*="#"]:not([href="#"])').on('click', function() {
    if (location.pathname.replace(/^\//,'') == this.pathname.replace(/^\//,'') && location.hostname == this.hostname) {

      let target = $(this.hash);
      if (target.length) {
        var top_space = 0;

        if( $('#header').length ) {
          top_space = $('#header').outerHeight();

          if( ! $('#header').hasClass('header-fixed') ) {
            top_space = top_space - 20;
          }
        }

        $('html, body').animate({
          scrollTop: target.offset().top - top_space
        }, 1500, 'easeInOutExpo');

        if ( $(this).parents('.nav-menu').length ) {
          $('.nav-menu .menu-active').removeClass('menu-active');
          $(this).closest('li').addClass('menu-active');
        }

        if ( $('body').hasClass('mobile-nav-active') ) {
          $('body').removeClass('mobile-nav-active');
          $('#mobile-nav-toggle i').toggleClass('fa-times fa-bars');
          $('#mobile-body-overly').fadeOut();
        }
        return false;
      }
    }
  });

  // Porfolio filter
  $("#portfolio-flters li").click ( function() {
    $("#portfolio-flters li").removeClass('filter-active');
    $(this).addClass('filter-active');

    var selectedFilter = $(this).data("filter");
    $("#portfolio-wrapper").fadeTo(100, 0);

    $(".portfolio-item").fadeOut().css('transform', 'scale(0)');

    setTimeout(function() {
      $(selectedFilter).fadeIn(100).css('transform', 'scale(1)');
      $("#portfolio-wrapper").fadeTo(300, 1);
    }, 300);
  });

  // jQuery counterUp
  $('[data-toggle="counter-up"]').counterUp({
    delay: 10,
    time: 1000
  });


  //Google Map
  let get_latitude = $('#google-map').data('latitude');
  let get_longitude = $('#google-map').data('longitude');

  function initialize_google_map() {
    let myLatlng = new google.maps.LatLng(get_latitude, get_longitude);
    let mapOptions = {
      zoom: 14,
      scrollwheel: false,
      center: myLatlng
    };
    let map = new google.maps.Map(document.getElementById('google-map'), mapOptions);
    let marker = new google.maps.Marker({
      position: myLatlng,
      map: map
    });
  }
  // google.maps.event.addDomListener(window, 'load', initialize_google_map);

// custom code


});

window.onload = init;

// let server_ip_map = new Map();
// let node_info;

function init()
{
    // getHostName();
    // getHostIp();
}
function requestConfig()
{
    $.ajax({
        url: "/index.html/request_config",
        type: "POST",
        dataType: "TEXT",
        success: function (data)
        {
            console.log('update config ack:',data)
            if('false' == data.toLowerCase())
            {
                alert("配置请求失败！");
            }
            else {
                let obj = JSON.parse(data);
                // $("#service-hostname").text('主机名:' + obj.host_name)
                let table=document.getElementById('config_table');

                for( let i = 0; i < 15; i++ )
                {
                    if(i < obj.pool_list.length)
                    {
                        if(obj.pool_list[i].enable)
                        {
                            let checkbox = '<input type="checkbox" class="btn ip-check" checked="checked" id="pool_check"></input>';
                            table.rows[i+1].cells[1].innerHTML = checkbox;
                        }
                        else
                        {
                            let checkbox = '<input type="checkbox" class="btn ip-check" id="pool_check"></input>';
                            table.rows[i+1].cells[1].innerHTML = checkbox;
                        }


                        let pool_id = '<input type="text"/>';
                        table.rows[i+1].cells[2].innerHTML = pool_id;
                        table.rows[i+1].cells[2].getElementsByTagName("INPUT")[0].value = obj.pool_list[i].id;

                        let max_queue_counts = '<input type="text"/>';
                        table.rows[i+1].cells[3].innerHTML = max_queue_counts;
                        table.rows[i+1].cells[3].getElementsByTagName("INPUT")[0].value = obj.pool_list[i].max_queue_counts;

                        let access_port = '<input type="text" />';
                        table.rows[i+1].cells[4].innerHTML = access_port;
                        table.rows[i+1].cells[4].getElementsByTagName("INPUT")[0].value = obj.pool_list[i].access_port;

                        let button = '<button type="button" class="btn btn-info" style="height:20px; width:80px;font-size:8px;padding: 1px;" onclick="delPool(this)" >删除</button>';
                        table.rows[i+1].cells[5].innerHTML = button;
                    }
                    else
                    {
                        table.rows[i+1].cells[0].innerHTML=(i+1);
                        for( let j = 1; j < 4; j++ )
                        {
                            table.rows[i+1].cells[j].innerHTML='---';
                        }
                        let button = '<button type="button" class="btn btn-info" style="height:20px; width:80px;font-size:8px;padding: 1px;" onclick="addPool(this)">添加</button>';
                        table.rows[i+1].cells[5].innerHTML = button;
                    }
                }
            }
            return false
        }
    })
}

function updateConfig()
{
    let table = document.getElementById('config_table');
    let pool_list=[];
    for (let i = 1; i < 16; i++)
    {
        let pool = {};
        try
        {
	        // let enable = table.rows[i].cells[1].getElementsByTagName("INPUT")[0].checked;
	        pool["enable"] = table.rows[i].cells[1].getElementsByTagName("INPUT")[0].checked;
	        // let name = table.rows[i].cells[2].getElementsByTagName("INPUT")[0].value;
	        pool["id"] = table.rows[i].cells[2].getElementsByTagName("INPUT")[0].value;
	        let max_queue_counts = table.rows[i].cells[3].getElementsByTagName("INPUT")[0].value;
	        pool["max_queue_counts"] = parseInt(max_queue_counts);
	        let access_port = table.rows[i].cells[4].getElementsByTagName("INPUT")[0].value;
	        pool["access_port"] = parseInt(access_port);
            pool_list.push(pool);

        }
        catch(err)
        {
            console.log(err.message);
        }
    }
    console.log(pool_list);
    let jsonObj = {"pool_list": pool_list};
    let json = JSON.stringify(jsonObj);
    console.log(json);

    $.ajax({
        url: "/index.html/update_config",
        type: "post",
        data: json,
        contentType: 'application/json; charset=UTF-8',
        dataType: "text",
        success: function (data)
        {
            console.log('update config ack:',data)
            if('true' == data.toLowerCase())
            {
                alert("配置更新成功！");
            }
            else
            {
                alert("配置更新失败！");
            }
            return false
        }
    })
}

function inputHostUrl()
{
    $("#connModal").modal({backdrop: 'static', keyboard: false});
}
function startService()
{
    let url = document.getElementById("host_url").value;
    $('#connModal').modal('hide');
    let jsonObj = {"url": url};
    console.log('startService:',JSON.stringify(jsonObj))
    $.ajax({
        url: "/index.html/start_service",
        type: "post",
        data: JSON.stringify(jsonObj),
        contentType: 'application/json; charset=UTF-8',
        dataType: "TEXT",
        success: function (data) {
            console.log('startService ack:',data)
            if('false' == data.toLowerCase()){
                alert("服务启动失败！");
            }
            else{
                let obj = JSON.parse(data);
                $("#service-hostname").text('主机名:' + obj.host_name)
                let table=document.getElementById('ip_table');
                for( let i = 0; i < 10; i++ )
                {
                    if(i < obj.host_infos.length)
                    {
                        table.rows[i+1].cells[1].innerHTML=obj.host_infos[i].eth;
                        table.rows[i+1].cells[2].innerHTML=obj.host_infos[i].ip;
                    }
                    else
                    {
                        table.rows[i+1].cells[0].innerHTML=(i+1);
                        for( let j = 1; j < 2; j++ )
                        {
                            table.rows[i+1].cells[j].innerHTML='---';
                        }
                    }
                }
                alert("服务启动成功！");
            }
            return false
        }
    })
}

function stopService() {
    $.ajax({
        url: "/index.html/stop_service",
        type: "post",
        dataType: "TEXT",
        success: function (data) {
            console.log('stopService:',data)
            if('true' == data.toLowerCase()){
                alert("服务终止成功！");
            }
            else{
                alert("服务终止失败！");
            }
            return false
        }
    })
}
function PrefixZero(num, n)
{
    return (Array(n).join(0) + num).slice(-n);
}
function addPool(id) {
    let row_index = id.parentNode.parentNode.rowIndex;
    let table = document.getElementById('config_table');

    let checkbox = '<input type="checkbox" class="btn ip-check" id="pool_check"/>';
    table.rows[row_index].cells[1].innerHTML = checkbox;

    let pool_id = '<input type="text" />';
    table.rows[row_index].cells[2].innerHTML = pool_id;
    table.rows[row_index].cells[2].getElementsByTagName("INPUT")[0].value = 'pool_' + PrefixZero(row_index, 3);

    let max_queue_counts = '<input type="text"/>';
    table.rows[row_index].cells[3].innerHTML = max_queue_counts;
    table.rows[row_index].cells[3].getElementsByTagName("INPUT")[0].value = '10';

    let access_port = '<input type="text"/>';
    table.rows[row_index].cells[4].innerHTML = access_port;
    let port = 12130 + row_index;
    table.rows[row_index].cells[4].getElementsByTagName("INPUT")[0].value = port.toString(10);

    let button = '<button type="button" class="btn btn-info" style="height:20px; width:80px;font-size:8px;padding: 1px;"  onclick="delPool(this)" >删除</button>';
    table.rows[row_index].cells[5].innerHTML = button;
}

function delPool(id) {
    let row_index = id.parentNode.parentNode.rowIndex;
    let table = document.getElementById('config_table');

    table.rows[row_index].cells[1].innerHTML = '---';
    table.rows[row_index].cells[2].innerHTML = '---';
    table.rows[row_index].cells[3].innerHTML = '---';
    table.rows[row_index].cells[4].innerHTML = '---';
    let button = '<button type="button" class="btn btn-info" style="height:20px; width:80px;font-size:8px;padding: 1px;" onclick="addPool(this)">添加</button>';
    table.rows[row_index].cells[5].innerHTML = button;
}

$(function()
{
    let html = "";
    for( let i = 0; i < 15; i++ )
    {
        html += "<tr>";
        html +=     "<td id=\"pool_no\">" + (i+1) + "</td>";
        html +=     "<td id=\"pool_enable\">---</td>";
        html +=     "<td id=\"pool_name\">---</td>";
        html +=     "<td id=\"max_queue\">---</td>";
        html +=     "<td id=\"access_port\">---</td>";
        html +=     "<td id=\"pool_edit\">---</td>";
        html += "</tr>";
    }
    $("#pool_config_table").html(html);
})

$(function()
{
    let html = "";
    for( let i = 0; i < 10; i++ )
    {
        html += "<tr>";
        html +=     "<td id=\"ip-no\">" + (i+1) + "</td>"
        html +=     "<td id=\"eth-card\">---</td>"
        html +=     "<td id=\"ip-value\">---</td>"
        html += "</tr>";
    }
    $("#ip_info").html(html);
})

$(function()
{
    // $("#config_table").on("click", ":button", function()
    // {
    //     $("#configModal").modal({backdrop: 'static', keyboard: false});
    //     var userName = $(this).parents("tr").find("#nodeName").text();
    //     $("#config_content").text(userName)
    // });
    // $("#ip_table").on("click", ":checkbox", function()
    // {
    //     let ip = $(this).parents("tr").find("#ip-value").text();
    //     let index = parseInt($(this).parents("tr").find("#ip-no").text(), 10);
    //     let check = $(this).is(':checked');
    //     if(check)
    //     {
    //         server_ip_map.set(index,ip);
    //     }
    //     else
    //     {
    //         server_ip_map.delete(index);
    //     }
    //     console.log(server_ip_map)
    // });
});

// function submitConf()
// {
//     let content = document.getElementById("config_content").value;
//     try {
//         $.parseJSON(content);
//     } catch (e) {
//         alert('Json字符串格式错误！')
//         return false;
//     }
//     alert('配置信息提交成功！')
//     return true;
// }
