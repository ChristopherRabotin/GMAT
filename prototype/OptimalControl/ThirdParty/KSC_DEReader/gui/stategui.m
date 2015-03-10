function varargout = stategui(varargin)
% STATEGUI Application M-file for stategui.fig
%    FIG = STATEGUI launch stategui GUI.
%    STATEGUI('callback_name', ...) invoke the named callback.

% Last Modified by GUIDE v2.0 12-Jul-2001 15:12:46

if nargin == 0  % LAUNCH GUI

	fig = openfig(mfilename,'reuse');

	% Generate a structure of handles to pass to callbacks, and store it. 
	handles = guihandles(fig);
    handles.state_data = []; % state data structure
	guidata(fig, handles);

	if nargout > 0
		varargout{1} = fig;
	end

elseif ischar(varargin{1}) % INVOKE NAMED SUBFUNCTION OR CALLBACK

	try
		[varargout{1:nargout}] = feval(varargin{:}); % FEVAL switchyard
	catch
		disp(lasterr);
	end

end


%| ABOUT CALLBACKS:
%| GUIDE automatically appends subfunction prototypes to this file, and 
%| sets objects' callback properties to call them through the FEVAL 
%| switchyard above. This comment describes that mechanism.
%|
%| Each callback subfunction declaration has the following form:
%| <SUBFUNCTION_NAME>(H, EVENTDATA, HANDLES, VARARGIN)
%|
%| The subfunction name is composed using the object's Tag and the 
%| callback type separated by '_', e.g. 'slider2_Callback',
%| 'figure1_CloseRequestFcn', 'axis1_ButtondownFcn'.
%|
%| H is the callback object's handle (obtained using GCBO).
%|
%| EVENTDATA is empty, but reserved for future use.
%|
%| HANDLES is a structure containing handles of components in GUI using
%| tags as fieldnames, e.g. handles.figure1, handles.slider2. This
%| structure is created at GUI startup using GUIHANDLES and stored in
%| the figure's application data using GUIDATA. A copy of the structure
%| is passed to each callback.  You can store additional information in
%| this structure at GUI startup, and you can change the structure
%| during callbacks.  Call guidata(h, handles) after changing your
%| copy to replace the stored original so that subsequent callbacks see
%| the updates. Type "help guihandles" and "help guidata" for more
%| information.
%|
%| VARARGIN contains any extra arguments you have passed to the
%| callback. Specify the extra arguments by editing the callback
%| property in the inspector. By default, GUIDE sets the property to:
%| <MFILENAME>('<SUBFUNCTION_NAME>', gcbo, [], guidata(gcbo))
%| Add any extra arguments after the last argument, before the final
%| closing parenthesis.



% --------------------------------------------------------------------
function varargout = pb_bfile_Callback(h, eventdata, handles, varargin)
% Get path to binary ephemeris file
[bfile,bpath]=uigetfile({'*.*','All Files (*,*)'}, ...
    'Choose Binary Ephemeris File');
set(handles.et_bfile,'String',cellstr([bpath,bfile]));
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = et_bfile_Callback(h, eventdata, handles, varargin)
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = pm_target_Callback(h, eventdata, handles, varargin)
if (get(handles.pm_target,'Value') > 13) % requesting nutation/libration
    set(handles.pm_units,'String', ...
        {'rad, rad/day';'rad, rad/s';'deg, deg/day';'deg, deg/s';'arcsec, arcsec/day'},'Value',1)
    set(handles.pm_coord,'Value',1) % nutations/librations wrt J2000 only
    set(handles.rb_csfix,'Value',1) % J2000 is a fixed CS
    set(handles.rb_float,'Value',0)
    set(handles.pm_center,'Value',get(handles.pm_target,'Value')) % Target/center meaningless for nut/lib
else % requesting position
    set(handles.pm_units,'String', ...
        {'AU, AU/day';'km, km/s';'nmi, nmi/s';'ft, ft/s'},'Value',1)
end
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = pm_center_Callback(h, eventdata, handles, varargin)

if (get(handles.pm_center,'Value') > 13) % requesting nutation/libration
    set(handles.pm_units,'String', ...
        {'rad, rad/day';'rad, rad/s';'deg, deg/day';'deg, deg/s';'arcsec, arcsec/day'},'Value',1)
    set(handles.pm_coord,'Value',1) % nutations/librations wrt J2000 only
    set(handles.pm_target,'Value',get(handles.pm_center,'Value')) % Target/center meaningless for nut/lib
    set(handles.rb_csfix,'Value',1) % J2000 is a fixed CS
    set(handles.rb_float,'Value',0)
else % requesting position
    set(handles.pm_units,'String', ...
        {'AU, AU/day';'km, km/s';'nmi, nmi/s';'ft, ft/s'},'Value',1)
end
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = pm_units_Callback(h, eventdata, handles, varargin)
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = pm_coord_Callback(h, eventdata, handles, varargin)
% If nutations/librations selected, allow only J2000 coordinate system
if (get(handles.pm_target,'Value') > 13 & get(handles.pm_center,'Value') >13 & ...
        get(handles.pm_coord,'Value') ~=1)
    set(handles.pm_coord,'Value',1);
    errordlg('Only J2000 coordinate system may be used for nutations and librations.','')
end
if (get(handles.pm_coord,'Value') == 1)
    set(handles.rb_csfix,'Value',1);
    set(handles.rb_float,'Value',0);
end

guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = pb_getstate_Callback(h, eventdata, handles, varargin)

eph_global % provide access to global variables

% Retrieve pertinent GUI data
bfile = char(get(handles.et_bfile,'String'));  % Binary file name (full path)
ntarg = get(handles.pm_target,'Value');  % Target body
ncent = get(handles.pm_center,'Value');  % Center body
uval  = get(handles.pm_units,'Value');   % Units
ncs   = get(handles.pm_coord,'Value');   % Coordinate system
jedcs = get(handles.et_csjed,'UserData'); % CS Epoch JED
jedst = get(handles.et_stjed,'UserData'); % Start time JED
jeden = get(handles.et_enjed,'UserData'); % End time JED
intv  = str2num(get(handles.et_interval,'String')); % Reporting Interval
intun = get(handles.pm_intunit,'Value');  % Interval units
csfix = get(handles.rb_csfix,'Value');    % Fixed CS?
%csrot = get(handles.rb_float,'Value');    % Rotating CS?

% Initialize binary ephemeris file
init_eph(bfile)

% Convert interval to days

cftime = [1/86400 % sec -> day
          1/1440  % min -> day
          1/24    % hr  -> day
          1       % day -> day
          365.25];% yr  -> day
      
tspan=jedst:intv*cftime(intun):jeden;

% For each date, use PLEPH to get state

np=3-(ntarg == 14 | ncent == 14);
rv=zeros(length(tspan),np);
vv=rv;

% Define ephemeris unit conversion factors

cfephm = [ %AU->AU          AU/d->AU/d           rad->rad       rad/d->rad/d
            1,              1,                   1,             1 
            %AU->km         AU/d->km/s           rad->rad       rad/d->rad/s
            AU,             AU/86400,            1,             1/86400   
            %AU->nmi        AU/d->nmi/s          rad->deg       rad/d->deg/d
            AU*1000/1852,   AU*1000/1852/86400,  180/pi,        180/pi
            %AU->ft         AU/d->ft/s           rad->deg       rad/d->deg/s
            AU*1000/.3048,  AU*1000/.3048/86400, 180/pi,        180/pi/86400
            %Not used       Not used             rad->arcsec    rad/d->arcsec/day
            0,              0,                   180/pi*3600,   180/pi*3600];

cfcol=[1 2]+2*(ntarg > 13 | ncent >13); % choose 1&2 or 3&4 columns
cfrow=get(handles.pm_units,'Value');

for i=1:length(tspan)
    [r,v]=pleph(tspan(i),ntarg,ncent,0);
    % if position & velocity, rotate output to desired coordinates
    if csfix
        tcs = jedcs;
    else
        tcs=tspan(i);
    end
    if (np==3 & ntarg ~=15) % do this only for position & velocity
        switch ncs
        case 1 % J2000
            f2r=eye(3);
        case 2 % EME of Date
            f2r=eme12eme2(tcs,2451545)';
        case 3 % True of Date
            load woolcon
            [me,de,dp]=woolard(tcs,w,c);
            f2r=eme2tod(me,de,dp)*eme12eme2(tcs,2451545)';
        case 4 % Earth fixed
            load woolcon
            [me,de,dp]=woolard(tcs,w,c);
            f2r=tod2ef(tcs,de,dp)*eme2tod(me,de,dp) ...
                *eme12eme2(tcs,2451545)';
        otherwise
            f2r=eye(3);
        end
        r=f2r*r; % transform r&v to rotating coordinate system
        v=f2r*v;
    end
    rv(i,:)=r(:)'*cfephm(cfrow,cfcol(1));
    vv(i,:)=v(:)'*cfephm(cfrow,cfcol(2));
end

% Put output in structure

pnames=get(handles.pm_target,'String');
d.target=char(pnames(get(handles.pm_target,'Value')));
d.center=char(pnames(get(handles.pm_center,'Value')));

unames=get(handles.pm_units,'String');
d.units=char(unames(get(handles.pm_units,'Value')));

csnames=get(handles.pm_coord,'String');
d.coordsys=char(csnames(get(handles.pm_coord,'Value')));

if ncs==1
    d.csmove='Inertial';
else
    if csfix
        d.csmove=sprintf('Inertial at Epoch:  JED = %f  (%s)',jedcs,datestr(jedcs-1721058.5,0));
    else
        d.csmove=['Rotates with ',d.coordsys];
    end
end
d.jed=tspan(:);
d.pos=rv;
d.vel=vv;

% Write output to output listbox

disp_state(d,handles.lb_output,handles)

% Make state available to rest of GUI
handles.state_data=d;
guidata(gcbo,handles)
% --------------------------------------------------------------------
function varargout = pb_export_Callback(h, eventdata, handles, varargin)
assignin('base','eph_state',handles.state_data)
guidata(gcbo,handles)
% --------------------------------------------------------------------
function varargout = pb_done_Callback(h, eventdata, handles, varargin)
close(gcbf)

% --------------------------------------------------------------------
function varargout = rb_csfix_Callback(h, eventdata, handles, varargin)
set(handles.rb_float,'Value',not(get(gcbo,'Value')));
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = rb_float_Callback(h, eventdata, handles, varargin)
set(handles.rb_csfix,'Value',not(get(gcbo,'Value')));
% If nutations/librations or J2000 CS chosen, allow only fixed coordinate system
if (get(handles.pm_target,'Value') > 13 & ...
        get(handles.pm_center,'Value') >13 |  ...
        get(handles.pm_coord,'Value') == 1)
    set(handles.rb_csfix,'Value',1);
    set(handles.rb_float,'Value',0);
    errordlg('J2000 is an inertial coordinate system.  It cannot rotate.','')
end
guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = update_jed(h, eventdata, handles, varargin)

% Coordinate system epoch
if ~isempty(findstr(get(gcbo,'Tag'),'_cs'))
    mo=get(handles.pm_csmon,'Value');
    da=str2num(char(get(handles.et_csdate,'String')));
    yr=str2num(char(get(handles.et_csyear,'String')));
    hr=str2num(char(get(handles.et_cshour,'String')));
    mi=str2num(char(get(handles.et_csmin,'String')));
    se=str2num(char(get(handles.et_cssec,'String')));
    eth=handles.et_csjed;
end

% Start time
if ~isempty(findstr(get(gcbo,'Tag'),'_st'))
    mo=get(handles.pm_stmon,'Value');
    da=str2num(char(get(handles.et_stdate,'String')));
    yr=str2num(char(get(handles.et_styear,'String')));
    hr=str2num(char(get(handles.et_sthour,'String')));
    mi=str2num(char(get(handles.et_stmin,'String')));
    se=str2num(char(get(handles.et_stsec,'String')));
    eth=handles.et_stjed;
end

% End time
if ~isempty(findstr(get(gcbo,'Tag'),'_en'))
    mo=get(handles.pm_enmon,'Value');
    da=str2num(char(get(handles.et_endate,'String')));
    yr=str2num(char(get(handles.et_enyear,'String')));
    hr=str2num(char(get(handles.et_enhour,'String')));
    mi=str2num(char(get(handles.et_enmin,'String')));
    se=str2num(char(get(handles.et_ensec,'String')));
    eth=handles.et_enjed;
end

% Get julian date
jed=date2jed([yr mo da hr mi se]);

% Set julian date value in edit text box
set(eth,'String',num2str(jed),'UserData',jed)

guidata(gcbo,handles)

% --------------------------------------------------------------------
function varargout = update_date(h, eventdata, handles, varargin)

date=jed2date(str2num(char(get(gcbo,'String'))));

% Coordinate system epoch
if ~isempty(findstr(get(gcbo,'Tag'),'_cs'))
    set(handles.pm_csmon,'Value',date(2))
    set(handles.et_csdate,'String',num2str(date(3)))
    set(handles.et_csyear,'String',num2str(date(1)))
    set(handles.et_cshour,'String',num2str(date(4)))
    set(handles.et_csmin,'String',num2str(date(5)))
    set(handles.et_cssec,'String',num2str(date(6)))
    set(handles.et_cssec,'UserData',date(6))
    eth=handles.et_csjed;
end

% Start time
if ~isempty(findstr(get(gcbo,'Tag'),'_st'))
    set(handles.pm_stmon,'Value',date(2))
    set(handles.et_stdate,'String',num2str(date(3)))
    set(handles.et_styear,'String',num2str(date(1)))
    set(handles.et_sthour,'String',num2str(date(4)))
    set(handles.et_stmin,'String',num2str(date(5)))
    set(handles.et_stsec,'String',num2str(date(6)))
    set(handles.et_stsec,'UserData',date(6))
    eth=handles.et_stjed;
end

% End time
if ~isempty(findstr(get(gcbo,'Tag'),'_en'))
    set(handles.pm_enmon,'Value',date(2))
    set(handles.et_endate,'String',num2str(date(3)))
    set(handles.et_enyear,'String',num2str(date(1)))
    set(handles.et_enhour,'String',num2str(date(4)))
    set(handles.et_enmin,'String',num2str(date(5)))
    set(handles.et_ensec,'String',num2str(date(6)))
    set(handles.et_ensec,'UserData',date(6))
    eth=handles.et_enjed;
end

% Set julian date value in edit text box
set(eth,'UserData',str2num(get(eth,'String')))

guidata(gcbo,handles)
% --------------------------------------------------------------------
function disp_state(d,h,handles)

eph_global
set(h,'FontName','Courier','FontSize',6)
s=[];

% Write header info
s=strvcat(s,sprintf('Data Interpolated From %s',TTL(1,:)));
s=strvcat(s,' ');

s=strvcat(s,sprintf('Target:          %s                Center:  %s', ...
    d.target,d.center));
s=strvcat(s,' ');

s=strvcat(s,sprintf('Coord. Sys.:     %s',d.coordsys));
s=strvcat(s,sprintf('CS Motion:       %s',d.csmove));
s=strvcat(s,' ');

s=strvcat(s,sprintf('Start JED:       %f   (%s)', ...
    d.jed(1),datestr(d.jed(1)-1721058.5,0)));
s=strvcat(s,sprintf('End JED:         %f   (%s)', ...
    d.jed(length(d.jed)),datestr(d.jed(length(d.jed))-1721058.5,0)));
tival=get(handles.et_interval,'String');
tivnames=get(handles.pm_intunit,'String');
tivunnum=get(handles.pm_intunit,'Value');
s=strvcat(s,sprintf('Time Interval:   %s %s', ...
    tival, char(tivnames(tivunnum))));
s=strvcat(s,' ');

cloc=findstr(d.units,',');
punit=fliplr(deblank(fliplr(deblank(d.units(1:cloc-1)))));
vunit=fliplr(deblank(fliplr(deblank(d.units(cloc+1:length(d.units))))));
s=strvcat(s,sprintf('Position Units:  %s         Velocity Units:  %s', ...
    punit,vunit));
s=strvcat(s,' ');

if (size(d.pos,2)==3)
    if isempty(findstr(d.target,'ibrat'))
        dim={'JED';sprintf('X (%s)',punit);sprintf('Y (%s)',punit);sprintf('Z (%s)',punit); ...
                sprintf('XD (%s)',vunit);sprintf('YD (%s)',vunit);sprintf('ZD (%s)',vunit)};
    else
         dim={'JED';sprintf('O (%s)',punit);sprintf('I (%s)',punit);sprintf('D (%s)',punit); ...
                sprintf('OD (%s)',vunit);sprintf('ID (%s)',vunit);sprintf('DD (%s)',vunit)};
    end   
    hdr=char(ones(1,154)*' ');
    ctr=12:22:154;
    for i=1:size(dim,1)
        strt=ctr(i)-ceil(size(char(dim{i}),2)/2);
        hdr(strt:strt+size(char(dim{i}),2)-1)=char(dim{i});
    end
    s=strvcat(s,hdr);
    for i=1:size(d.jed,1)
        s=strvcat(s,sprintf(' %21.13e %21.13e %21.13e %21.13e %21.13e %21.13e %21.13e\n', ...
            [d.jed(i) d.pos(i,:) d.vel(i,:)]'));
    end
else
    dim={'JED';sprintf('dpsi (%s)',punit);sprintf('deps (%s)',punit); ...
            sprintf('ddpsi (%s)',vunit);sprintf('ddeps (%s)',vunit)};
    hdr=char(ones(1,154)*' ');
    ctr=12:22:110;
    for i=1:size(dim,1)
        strt=ctr(i)-ceil(size(char(dim{i}),2)/2);
        hdr(strt:strt+size(char(dim{i}),2)-1)=char(dim{i});
    end
    s=strvcat(s,hdr);
    for i=1:size(d.jed,1)
        s=strvcat(s,sprintf(' %21.13e %21.13e %21.13e %21.13e %21.13e\n', ...
            [d.jed(i) d.pos(i,:) d.vel(i,:)]'));
    end
end

% Display
set(h,'String',cellstr(s));
guidata(gcbo,handles)