function r = asin(a)
%ASIN         Taylor inverse sine  asin(a)
%
%Thanks to George Corliss for providing the scheme for the Taylor expansion
%

% written  06/03/09     S.M. Rump
%

  e = 1e-30;
  if 1+e==1-e                   % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  global INTLAB_TAYLOR_ORDER
  K = INTLAB_TAYLOR_ORDER;
  
  r = a;
  dt = a.t;
  N = size(a.t,2);
  dt(1,:) = sqrt(1-a.t(1,:).^2);     % sqrt(1-a^2)
  dt2 = (-2)*dt(1,:);
  for j=2:K
    dt(j,:) = ( sum(a.t(1:j,:).*a.t(j:-1:1,:),1) + sum(dt(2:j-1,:).*dt(j-1:-1:2,:),1) ) ./ dt2;
  end
  r.t(1,:) = asin(a.t(1,:));
  r.t(2,:) = a.t(2,:) ./ dt(1,:);
  for j=2:K
    r.t(j+1,:) = ( j*a.t(j+1,:) - sum(repmat((1:j-1)',1,N).*r.t(2:j,:).*dt(j:-1:2,:),1) ) ./ ( j*dt(1,:) );
  end

  if rndold
    setround(rndold)
  end
