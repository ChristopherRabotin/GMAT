function handles=progressbar(ttlstr)
handles.hf=figure('Position',[340 270 400 100],'MenuBar','none', ...
    'NumberTitle','off','Name',ttlstr,'Resize','off');
handles.ha=axes;
handles.hb=barh(0,100);
set(handles.ha,'Visible','off')
hold on
handles.hr=barh(0,0,'r');
handles.ht=text(50,0,'0%','Color','w','FontSize',18,'FontWeight','bold', ...
    'HorizontalAlignment','center','VerticalAlignment','middle');
